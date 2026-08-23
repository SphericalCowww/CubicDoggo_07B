#include <iostream>
#include <chrono>
#include <thread>
#include "rclcpp/rclcpp.hpp"
#include "my_robot_firmware/hardware_interface_cubic_doggo_st3215.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cubic_doggo_namespace {
    hardware_interface::CallbackReturn HardwareInterfaceST3215_cubic_doggo::on_init(
        const hardware_interface::HardwareComponentInterfaceParams &params) 
    {
        if (hardware_interface::SystemInterface::on_init(params) != hardware_interface::CallbackReturn::SUCCESS) {
            return hardware_interface::CallbackReturn::ERROR;
        }
        RCLCPP_INFO(get_logger(), "hardware_interface:on_init()");

        try {
            port_name_front_ = params.hardware_info.hardware_parameters.at("port_name_front");
            port_name_back_  = params.hardware_info.hardware_parameters.at("port_name_back");
            baud_rate_ = std::stoi(params.hardware_info.hardware_parameters.at("baud_rate"));
        } catch (const std::out_of_range& errorMsg) {
            RCLCPP_ERROR(get_logger(), "hardware_interface:on_init(): missing required parameter in URDF");
            return hardware_interface::CallbackReturn::ERROR;
        }
        RCLCPP_INFO(get_logger(), "hardware_interface:on_init(): st3215 opening port %s and %s at %d baud", 
                                  port_name_front_.c_str(), port_name_back_.c_str(), baud_rate_);
        if (!sts_wb_[0].begin(baud_rate_, port_name_front_)) {
            RCLCPP_ERROR(get_logger(), "hardware_interface:on_init(): failed to open the port %s!",
                                       port_name_front.c_str());
            return hardware_interface::CallbackReturn::ERROR;
        }
        if (!sts_wb_[1].begin(baud_rate_, port_name_back_)) {
            RCLCPP_ERROR(get_logger(), "hardware_interface:on_init(): failed to open the port %s!",
                                       port_name_back_.c_str());
            return hardware_interface::CallbackReturn::ERROR;
        }
        RCLCPP_INFO(get_logger(), "hardware_interface:on_init(): initialize with baud rate: %d", baud_rate_);


        /////////// see: src/my_robot_description/urdf/cubic_doggo.ros2_control.xacro
        servo_channels_[0]  = std::stoi(params.hardware_info.hardware_parameters.at("servo1_channel_FL"));
        servo_channels_[1]  = std::stoi(params.hardware_info.hardware_parameters.at("servo2_channel_FL"));
        servo_channels_[2]  = std::stoi(params.hardware_info.hardware_parameters.at("servo3_channel_FL"));
        servo_channels_[3]  = std::stoi(params.hardware_info.hardware_parameters.at("servo1_channel_FR"));
        servo_channels_[4]  = std::stoi(params.hardware_info.hardware_parameters.at("servo2_channel_FR"));
        servo_channels_[5]  = std::stoi(params.hardware_info.hardware_parameters.at("servo3_channel_FR"));
        servo_channels_[6]  = std::stoi(params.hardware_info.hardware_parameters.at("servo1_channel_BL"));
        servo_channels_[7]  = std::stoi(params.hardware_info.hardware_parameters.at("servo2_channel_BL"));
        servo_channels_[8]  = std::stoi(params.hardware_info.hardware_parameters.at("servo3_channel_BL"));
        servo_channels_[9]  = std::stoi(params.hardware_info.hardware_parameters.at("servo1_channel_BR"));
        servo_channels_[10] = std::stoi(params.hardware_info.hardware_parameters.at("servo2_channel_BR"));
        servo_channels_[11] = std::stoi(params.hardware_info.hardware_parameters.at("servo3_channel_BR"));
        joint_names = {"servo1_servo1_padding_FL", 
                       "servo2_servo2_padding_FL", 
                       "servo3_calfFeet_FL",
                       "servo1_servo1_padding_FR",
                       "servo2_servo2_padding_FR",
                       "servo3_calfFeet_FR",
                       "servo1_servo1_padding_BL",
                       "servo2_servo2_padding_BL",
                       "servo3_calfFeet_BL",
                       "servo1_servo1_padding_BR",
                       "servo2_servo2_padding_BR",
                       "servo3_calfFeet_BR"};
        rad_pos_init_[0]  = 3.14;                      
        rad_pos_init_[1]  = 2.54;                      //M_PI - M_PI/4.0;
        rad_pos_init_[2]  = 4.14;                      //M_PI - M_PI/8.0;
        rad_pos_init_[3]  = 3.14;                      
        rad_pos_init_[4]  = 3.74;                      //M_PI + M_PI/4.0;
        rad_pos_init_[5]  = 2.14;                      //M_PI + M_PI/8.0;
        rad_pos_init_[6]  = 3.14;                     
        rad_pos_init_[7]  = 2.54;                      //M_PI - M_PI/4.0;
        rad_pos_init_[8]  = 4.14;                      //M_PI - M_PI/8.0;
        rad_pos_init_[9]  = 3.14;                      
        rad_pos_init_[10] = 3.74;                      //M_PI + M_PI/4.0;
        rad_pos_init_[11] = 2.14;                      //M_PI + M_PI/8.0;
        ///////////


        return hardware_interface::CallbackReturn::SUCCESS;     
    }
    hardware_interface::CallbackReturn HardwareInterfaceST3215_cubic_doggo::on_configure( 
        const rclcpp_lifecycle::State & previous_state) 
    {
        RCLCPP_INFO(get_logger(), "hardware_interface:on_configure()");
        (void) previous_state;
        
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            sts_idx_ = (servo_idx < 6) ? 0 : 1;
            if (!sts_wb_[sts_idx_].Ping(servo_channels_[servo_idx])) {
                RCLCPP_ERROR(get_logger(), "hardware_interface:on_configure(): failed to ping!");
                return hardware_interface::CallbackReturn::ERROR;
            } else {
                RCLCPP_INFO(get_logger(), "hardware_interface:on_configure(): pinging id: %d",
                                          servo_channels_[servo_idx]);
            }
            // torque enable
            if (!sts_wb_[sts_idx_].EnableTorque(servo_channels_[servo_idx], 1)) {
                RCLCPP_ERROR(get_logger(), "hardware_interface:on_configure(): failed to enable torque!");
                return hardware_interface::CallbackReturn::ERROR;
            } else {
                RCLCPP_INFO(get_logger(), "hardware_interface:on_configure(): torque enable for ch %d", 
                                          servo_channels_[servo_idx]);
            }
        }
        return hardware_interface::CallbackReturn::SUCCESS;
    }
    hardware_interface::CallbackReturn HardwareInterfaceST3215_cubic_doggo::on_activate(
        const rclcpp_lifecycle::State & previous_state) 
    {
        RCLCPP_INFO(get_logger(), "hardware_interface:on_activate()");
        (void) previous_state;
       
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) initialize_servo_(servo_idx);
        sts_wb_[0].SyncWritePosEx(&servo_channels_[0], servo_N_/2, &sts_pos[0], &sts_vel[0], &sts_acc[0]);
        sts_wb_[1].SyncWritePosEx(&servo_channels_[servo_N_/2], servo_N_/2, 
                                  &sts_pos[servo_N_/2], &sts_vel[servo_N_/2], &sts_acc[servo_N_/2]);
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            set_state(joint_names[servo_idx]+"/position", rad_pos_[servo_idx]);
        }

        return hardware_interface::CallbackReturn::SUCCESS;
    }
    hardware_interface::CallbackReturn HardwareInterfaceST3215_cubic_doggo::on_deactivate(
        const rclcpp_lifecycle::State & previous_state) 
    {
        RCLCPP_INFO(get_logger(), "hardware_interface:on_deactivate()");
        (void) previous_state;

        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) initialize_servo_(servo_idx);
        sts_wb_[0].SyncWritePosEx(&servo_channels_[0], servo_N_/2, &sts_pos[0], &sts_vel[0], &sts_acc[0]);
        sts_wb_[1].SyncWritePosEx(&servo_channels_[servo_N_/2], servo_N_/2,
                                  &sts_pos[servo_N_/2], &sts_vel[servo_N_/2], &sts_acc[servo_N_/2]);
 
        return hardware_interface::CallbackReturn::SUCCESS;
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    hardware_interface::return_type HardwareInterfaceST3215_cubic_doggo::read(
        const rclcpp::Time & time, const rclcpp::Duration & period) 
    {
        RCLCPP_DEBUG(get_logger(), "hardware_interface:read()");
        (void) period;
        if (write_first_call_ == true) {
            start_time_ = time;
            write_first_call_ = false;
        }
        rclcpp::Duration lifetime = time - start_time_;
    
        std::thread thread0(&HardwareInterfaceST3215_cubic_doggo::read_controller_range, this, 0);
        std::thread thread1(&HardwareInterfaceST3215_cubic_doggo::read_controller_range, this, 1);
        thread0.join(); thread1.join();
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            rad_pos_[servo_idx] = (double) sts_pos_[servo_idx]*(2.0*M_PI)/(MAX_POSITION+1-MIN_POSITION);
            rad_vel_[servo_idx] = (double) sts_vel_[servo_idx]*(2.0*M_PI)/(MAX_POSITION+1-MIN_POSITION);
            rad_eff_[servo_idx] = (double) static_cast<int16_t>(sts_eff[servo_idx]); // for Present_Load 
            // see: src/my_robot_description/urdf/cubic_doggo.ros2_control.xacro
            set_state(joint_names[servo_idx]+"/position", rad_pos_[servo_idx]);
            set_state(joint_names[servo_idx]+"/velocity", rad_vel_[servo_idx]);
            set_state(joint_names[servo_idx]+"/effort",   rad_eff_[servo_idx]);
        }
        return hardware_interface::return_type::OK;
    }
    hardware_interface::return_type HardwareInterfaceST3215_cubic_doggo::write(
        const rclcpp::Time & time, const rclcpp::Duration & period) 
    {
        RCLCPP_DEBUG(get_logger(), "hardware_interface:write()");
        (void) period; 
        
        // see: src/my_robot_description/urdf/cubic_doggo.ros2_control.xacro
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            rad_pos_[servo_idx] = get_command(joint_names[servo_idx]+"/position");
            if (std::isnan(rad_pos_[servo_idx]) == true) initialize_servo_(servo_idx); 
            sts_pos_[servo_idx] = static_cast<s16>(std::round(rad_pos_[servo_id]
                                                             *(MAX_POSITION+1-MIN_POSITION)/(2.0*M_PI)));
        }
        sts_wb_[0].SyncWritePosEx(&servo_channels_[0], servo_N_/2, &sts_pos[0], &sts_vel[0], &sts_acc[0]);
        sts_wb_[1].SyncWritePosEx(&servo_channels_[servo_N_/2], servo_N_/2,
                                  &sts_pos[servo_N_/2], &sts_vel[servo_N_/2], &sts_acc[servo_N_/2]);

        return hardware_interface::return_type::OK;
    }
    HardwareInterfaceST3215_cubic_doggo::~HardwareInterfaceST3215_cubic_doggo()
    {
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            sts_idx_ = (servo_idx < 6) ? 0 : 1;
            sts_wb_[sts_idx_].EnableTorque(servo_channels_[servo_idx], 0);
        }
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void HardwareInterfaceST3215_cubic_doggo::initialize_servo_(uint8_t servo_id) {
        rad_pos_[servo_id] = rad_pos_init_[servo_id];
        sts_pos_[servo_id] =static_cast<s16>(std::round(rad_pos_[servo_id]*(MAX_POSITION+1-MIN_POSITION)/(2.0*M_PI)));
        rad_vel_[servo_id] = 0.0;
        sts_vel_[servo_id] = 0;
        rad_eff_[servo_id] = 0.0;
        sts_eff_[servo_id] = 0;
    }
    void HardwareInterfaceST3215_cubic_doggo::read_memory_(uint8_t servo_id) {
        sts_idx_ = (servo_idx < 6) ? 0 : 1;
        uint8_t raw_data[6];
        if (sts_wb_[0].ReadMem(servo_id, 56, raw_data, 6) == 6) {
            sts_pos_[i]     = *(s16*) &raw_data[0];
            sts_vel_[i]     = *(s16*) &raw_data[2];
            sts_efforts_[i] = *(s16*) &raw_data[4];
        }
    }    
    void HardwareInterfaceST3215_cubic_doggo::read_controller_range(std::size_t ctrl_idx) {
    std::size_t servo_stard_idx = (ctrl_idx == 0) ? 0 : servo_N_/2;
    std::size_t servo_end_idx   = start + servo_N_/2;
    for (std::size_t servo_idx = servo_stard_idx; servo_idx < servo_end_idx; servo_idx++) {
        uint8_t raw_data[6];
        if (sts_wb_[ctrl_idx].ReadMem(servo_channels_[servo_idx], 56, raw_data, 6) == 6) {
            sts_pos_[servo_idx] = *(s16*) &raw_data[0];
            sts_vel_[servo_idx] = *(s16*) &raw_data[2];
            sts_eff_[servo_idx] = *(s16*) &raw_data[4];
        } else {
            sts_eff_[servo_idx] = -32768; 
        }
    }
}

}
#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(cubic_doggo_namespace::HardwareInterfaceST3215_cubic_doggo,hardware_interface::SystemInterface)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
