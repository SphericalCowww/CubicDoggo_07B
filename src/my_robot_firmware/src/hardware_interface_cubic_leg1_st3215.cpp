#include <iostream>
#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "my_robot_firmware/hardware_interface_cubic_leg1_st3215.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cubic_leg1_namespace {
    hardware_interface::CallbackReturn HardwareInterfaceST3215_cubic_leg1::on_init(
        const hardware_interface::HardwareComponentInterfaceParams &params) 
    {
        if (hardware_interface::SystemInterface::on_init(params) != hardware_interface::CallbackReturn::SUCCESS) {
            return hardware_interface::CallbackReturn::ERROR;
        }
        RCLCPP_INFO(get_logger(), "hardware_interface:on_init()");

        try {
            port_name_ = params.hardware_info.hardware_parameters.at("port_name");
            baud_rate_ = std::stoi(params.hardware_info.hardware_parameters.at("baud_rate"));
        } catch (const std::out_of_range& errorMsg) {
            RCLCPP_ERROR(get_logger(), "hardware_interface:on_init(): missing required parameter in URDF");
            return hardware_interface::CallbackReturn::ERROR;
        }
        RCLCPP_INFO(get_logger(), "hardware_interface:on_init(): st3215 opening port %s at %d baud", 
                                  port_name_.c_str(), baud_rate_);
        if (!sts_wb_.begin(baud_rate_, port_name_.c_str())) {
            RCLCPP_ERROR(get_logger(), "hardware_interface:on_init(): failed to open the port %s!",
                                       port_name_.c_str());
            return hardware_interface::CallbackReturn::ERROR;
        }

        /////////// see: src/my_robot_description/urdf/cubic_leg1.ros2_control.xacro
        servo_channels_[0] = std::stoi(params.hardware_info.hardware_parameters.at("servo1_channel_FL"));
        servo_channels_[1] = std::stoi(params.hardware_info.hardware_parameters.at("servo2_channel_FL"));
        servo_channels_[2] = std::stoi(params.hardware_info.hardware_parameters.at("servo3_channel_FL"));
        joint_names = {"servo1_servo1_padding_FL", 
                       "servo2_servo2_padding_FL", 
                       "servo3_calfFeet_FL"};
        rad_pos_init_[0]  = 3.14;                      
        rad_pos_init_[1]  = 2.54;                      //M_PI - M_PI/4.0;
        rad_pos_init_[2]  = 4.14;                      //M_PI - M_PI/8.0;

        ///////////


        return hardware_interface::CallbackReturn::SUCCESS;     
    }
    hardware_interface::CallbackReturn HardwareInterfaceST3215_cubic_leg1::on_configure( 
        const rclcpp_lifecycle::State & previous_state) 
    {
        RCLCPP_INFO(get_logger(), "hardware_interface:on_configure()");
        (void) previous_state;
        
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            if (!sts_wb_.Ping(servo_channels_[servo_idx])) {
                RCLCPP_ERROR(get_logger(), "hardware_interface:on_configure(): failed to ping!");
                return hardware_interface::CallbackReturn::ERROR;
            } else {
                RCLCPP_INFO(get_logger(), "hardware_interface:on_configure(): pinging id: %d",
                                          servo_channels_[servo_idx]);
            }
            // torque enable
            if (!sts_wb_.EnableTorque(servo_channels_[servo_idx], 1)) {
                RCLCPP_ERROR(get_logger(), "hardware_interface:on_configure(): failed to enable torque!");
                return hardware_interface::CallbackReturn::ERROR;
            } else {
                RCLCPP_INFO(get_logger(), "hardware_interface:on_configure(): torque enable for ch %d", 
                                          servo_channels_[servo_idx]);
            }
        }
        return hardware_interface::CallbackReturn::SUCCESS;
    }
    hardware_interface::CallbackReturn HardwareInterfaceST3215_cubic_leg1::on_activate(
        const rclcpp_lifecycle::State & previous_state) 
    {
        RCLCPP_INFO(get_logger(), "hardware_interface:on_activate()");
        (void) previous_state;
       
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) initialize_servo_(servo_idx);
        sts_wb_.SyncWritePosEx(&servo_channels_[0], servo_N_, &sts_pos_[0], &sts_vel_[0], &sts_acc_[0]);
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            set_state(joint_names[servo_idx]+"/position", rad_pos_[servo_idx]);
        }

        return hardware_interface::CallbackReturn::SUCCESS;
    }
    hardware_interface::CallbackReturn HardwareInterfaceST3215_cubic_leg1::on_deactivate(
        const rclcpp_lifecycle::State & previous_state) 
    {
        RCLCPP_INFO(get_logger(), "hardware_interface:on_deactivate()");
        (void) previous_state;

        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) initialize_servo_(servo_idx);
        sts_wb_.SyncWritePosEx(&servo_channels_[0], servo_N_, &sts_pos_[0], &sts_vel_[0], &sts_acc_[0]);
        
        return hardware_interface::CallbackReturn::SUCCESS;
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    hardware_interface::return_type HardwareInterfaceST3215_cubic_leg1::read(
        const rclcpp::Time & time, const rclcpp::Duration & period) 
    {
        RCLCPP_DEBUG(get_logger(), "hardware_interface:read()");
        (void) time; 
        (void) period;
        //if (write_first_call_ == true) {
        //    start_time_ = time;
        //    write_first_call_ = false;
        //}
        //rclcpp::Duration lifetime = time - start_time_;
   


        // WARNING: TURN OFF CURRENT READ CODE DUE TO LATENCY
        //read_controller_range();



        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            rad_pos_[servo_idx] = (double) (sts_range_-sts_pos_[servo_idx])*(2.0*M_PI)/sts_range_;
            rad_vel_[servo_idx] = (double) -sts_vel_[servo_idx]            *(2.0*M_PI)/sts_range_;
            rad_eff_[servo_idx] = (double) static_cast<int16_t>(sts_eff_[servo_idx]); // for Present_Load 
            // see: src/my_robot_description/urdf/cubic_leg1.ros2_control.xacro
            set_state(joint_names[servo_idx]+"/position", rad_pos_[servo_idx]);
            set_state(joint_names[servo_idx]+"/velocity", rad_vel_[servo_idx]);
            set_state(joint_names[servo_idx]+"/effort",   rad_eff_[servo_idx]);
        }
        return hardware_interface::return_type::OK;
    }
    hardware_interface::return_type HardwareInterfaceST3215_cubic_leg1::write(
        const rclcpp::Time & time, const rclcpp::Duration & period) 
    {
        RCLCPP_DEBUG(get_logger(), "hardware_interface:write()");
        (void) time;  
        (void) period; 
        
        // see: src/my_robot_description/urdf/cubic_leg1.ros2_control.xacro
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            rad_pos_[servo_idx] = get_command(joint_names[servo_idx]+"/position");
            if (std::isnan(rad_pos_[servo_idx]) == true) initialize_servo_(servo_idx); 
            sts_pos_[servo_idx] = static_cast<s16>(std::round((2.0*M_PI-rad_pos_[servo_idx])*sts_range_/(2.0*M_PI)));
        }
        sts_wb_.SyncWritePosEx(&servo_channels_[0], servo_N_, &sts_pos_[0], &sts_vel_[0], &sts_acc_[0]);

        return hardware_interface::return_type::OK;
    }
    HardwareInterfaceST3215_cubic_leg1::~HardwareInterfaceST3215_cubic_leg1()
    {
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            sts_wb_.EnableTorque(servo_channels_[servo_idx], 0);
        }
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void HardwareInterfaceST3215_cubic_leg1::initialize_servo_(uint8_t servo_idx) {
        rad_pos_[servo_idx] = rad_pos_init_[servo_idx];
        rad_vel_[servo_idx] = 0.0; 
        rad_eff_[servo_idx] = 0.0;
        sts_pos_[servo_idx] = static_cast<s16>(std::round((2.0*M_PI-rad_pos_[servo_idx])*sts_range_/(2.0*M_PI)));
        sts_vel_[servo_idx] = 0;
        sts_acc_[servo_idx] = 0;
        sts_eff_[servo_idx] = 0;
    }
    void HardwareInterfaceST3215_cubic_leg1::read_controller_range() {
        //auto t0 = std::chrono::steady_clock::now();     
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            uint8_t raw_data[6];
            if (sts_wb_.Read(servo_channels_[servo_idx], 56, raw_data, 6) == 6) {
                sts_pos_[servo_idx] = *(s16*) &raw_data[0];
                sts_vel_[servo_idx] = *(s16*) &raw_data[2];
                sts_eff_[servo_idx] = *(s16*) &raw_data[4];
            } else {
                sts_eff_[servo_idx] = -32768; 
            }
        }
        //auto dt = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
        //RCLCPP_INFO(get_logger(), "HERE: took %.3f ms", 1000*dt);
    }
    /*
    bool HardwareInterfaceST3215_cubic_leg1::sync_read_controller_range() {
        // 1. Assemble the SYNC_READ packet
        uint8_t packet[64];
        int idx = 0;
        packet[idx++] = 0xFF;
        packet[idx++] = 0xFF;
        packet[idx++] = 0xFE;       // Broadcast ID
        uint8_t len_pos = idx;      // Remember where to put length
        idx++;                      // Skip length for now
        packet[idx++] = 0x82;       // SYNC_READ instruction
        packet[idx++] = 0x38;       // Starting Address (Position L)
        packet[idx++] = 0x00;       // Address High (0x38)
        packet[idx++] = 0x06;       // Data length per servo (6 bytes)

        for (size_t i = 0; i < servo_N_; i++) {
            packet[idx++] = servo_channels_[i]; // Add servo IDs
        }

        packet[len_pos] = idx - len_pos - 1; // Calculate length (bytes after length byte)
    
        // Calculate Checksum (exclude header 0xFF 0xFF and Broadcast ID 0xFE)
        uint8_t checksum = 0;
        for (int i = 2; i < idx; i++) { // Start from Broadcast ID
            checksum += packet[i];
        }
        packet[idx++] = ~checksum & 0xFF;

        // 2. Send the packet
        // NOTE: Replace 'pSerial->fd' with the actual file descriptor/handle of your sts_wb_
        if (write(sts_wb_.pSerial->fd, packet, idx) != idx) {
            return false;
        }

        // 3. Read ALL responses at once (CRITICAL: Do not loop calling read)
        uint8_t rxbuf[256];
        int rxlen = read(sts_wb_.pSerial->fd, rxbuf, sizeof(rxbuf));
        if (rxlen <= 0) return false;

        // 4. Parse response packets from the buffer
        int pos = 0;
        int parsed = 0;
        while (pos + 6 <= rxlen) {
            // Look for packet header
            if (rxbuf[pos] != 0xFF || rxbuf[pos+1] != 0xFF) { 
                pos++; 
                continue; 
            }
        
            uint8_t id = rxbuf[pos+2];
            uint8_t rlen = rxbuf[pos+3]; // Length of remaining bytes in this packet
            if (pos + 4 + rlen > rxlen) break; // Incomplete packet

            // Find the index of this servo ID
            int si = -1;
            for (size_t i = 0; i < servo_N_; i++) {
                if (servo_channels_[i] == id) { si = i; break; }
            }

            // ST Series response data starts after: Header(2) + ID(1) + Len(1) + Error(1) = 5 bytes offset
            // The official example shows the response length (0x0A) includes Error + 8 data bytes + Checksum
            // For 6 bytes of data (Pos+Vel+Load), length should be 1(Err)+6(Data)+1(Chk) = 8 (0x08)
            if (si >= 0 && rlen >= 8) { 
                uint8_t* data = &rxbuf[pos+5]; // Skip FF FF ID LEN ERR
                sts_pos_[si] = *(int16_t*)&data[0];
                sts_vel_[si] = *(int16_t*)&data[2];
                sts_eff_[si] = *(int16_t*)&data[4];
                parsed++;
            }
            pos += 4 + rlen; // Move to the next packet
        }
        return parsed == (int)servo_N_;
    }
    */
}
#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(cubic_leg1_namespace::HardwareInterfaceST3215_cubic_leg1, hardware_interface::SystemInterface)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
