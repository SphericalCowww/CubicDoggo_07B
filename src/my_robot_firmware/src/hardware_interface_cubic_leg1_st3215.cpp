#include <iostream>
#include <chrono>
#include <thread>
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
            port_name_front_ = params.hardware_info.hardware_parameters.at("port_name_front");
            port_name_back_  = params.hardware_info.hardware_parameters.at("port_name_back");
            baud_rate_ = std::stoi(params.hardware_info.hardware_parameters.at("baud_rate"));
        } catch (const std::out_of_range& errorMsg) {
            RCLCPP_ERROR(get_logger(), "hardware_interface:on_init(): missing required parameter in URDF");
            return hardware_interface::CallbackReturn::ERROR;
        }
        RCLCPP_INFO(get_logger(), "hardware_interface:on_init(): st3215 opening port %s and %s at %d baud", 
                                  port_name_front_.c_str(), port_name_back_.c_str(), baud_rate_);
        if (!sts_wb_[0].begin(baud_rate_, port_name_front_.c_str())) {
            RCLCPP_ERROR(get_logger(), "hardware_interface:on_init(): failed to open the port %s!",
                                       port_name_front_.c_str());
            return hardware_interface::CallbackReturn::ERROR;
        }

        /////////// see: src/my_robot_description/urdf/cubic_leg1.ros2_control.xacro
        servo_channels_[0]  = std::stoi(params.hardware_info.hardware_parameters.at("servo1_channel_FL"));
        servo_channels_[1]  = std::stoi(params.hardware_info.hardware_parameters.at("servo2_channel_FL"));
        servo_channels_[2]  = std::stoi(params.hardware_info.hardware_parameters.at("servo3_channel_FL"));
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
    hardware_interface::CallbackReturn HardwareInterfaceST3215_cubic_leg1::on_activate(
        const rclcpp_lifecycle::State & previous_state) 
    {
        RCLCPP_INFO(get_logger(), "hardware_interface:on_activate()");
        (void) previous_state;
       
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) initialize_servo_(servo_idx);
        sts_wb_[0].SyncWritePosEx(&servo_channels_[0], servo_N_/2, &sts_pos_[0], &sts_vel_[0], &sts_acc_[0]);
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
        sts_wb_[0].SyncWritePosEx(&servo_channels_[0], servo_N_/2, &sts_pos_[0], &sts_vel_[0], &sts_acc_[0]);
        
        return hardware_interface::CallbackReturn::SUCCESS;
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    hardware_interface::return_type HardwareInterfaceST3215_cubic_leg1::read(
        const rclcpp::Time & time, const rclcpp::Duration & period) 
    {
        RCLCPP_DEBUG(get_logger(), "hardware_interface:read()");
        (void) period;
        if (write_first_call_ == true) {
            start_time_ = time;
            write_first_call_ = false;
        }
        rclcpp::Duration lifetime = time - start_time_;
    
        std::thread thread0(&HardwareInterfaceST3215_cubic_leg1::read_controller_range, this, 0);
        thread0.join();
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            rad_pos_[servo_idx] = (double) sts_pos_[servo_idx]*(2.0*M_PI)/(MAX_POSITION+1-MIN_POSITION);
            rad_vel_[servo_idx] = (double) sts_vel_[servo_idx]*(2.0*M_PI)/(MAX_POSITION+1-MIN_POSITION);
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
        (void) period; 
        
        // see: src/my_robot_description/urdf/cubic_leg1.ros2_control.xacro
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            rad_pos_[servo_idx] = get_command(joint_names[servo_idx]+"/position");
            if (std::isnan(rad_pos_[servo_idx]) == true) initialize_servo_(servo_idx); 
            sts_pos_[servo_idx] = static_cast<s16>(std::round(rad_pos_[servo_idx]
                                                             *(MAX_POSITION+1-MIN_POSITION)/(2.0*M_PI)));
        }
        sts_wb_[0].SyncWritePosEx(&servo_channels_[0], servo_N_/2, &sts_pos_[0], &sts_vel_[0], &sts_acc_[0]);

        return hardware_interface::return_type::OK;
    }
    HardwareInterfaceST3215_cubic_leg1::~HardwareInterfaceST3215_cubic_leg1()
    {
        for (std::size_t servo_idx = 0; servo_idx < servo_N_; servo_idx++) {
            sts_idx_ = (servo_idx < 6) ? 0 : 1;
            sts_wb_[sts_idx_].EnableTorque(servo_channels_[servo_idx], 0);
        }
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void HardwareInterfaceST3215_cubic_leg1::initialize_servo_(uint8_t servo_idx) {
        rad_pos_[servo_idx] = rad_pos_init_[servo_idx];
        sts_pos_[servo_idx] = static_cast<s16>(std::round(rad_pos_[servo_idx]
                                              *(MAX_POSITION+1-MIN_POSITION)/(2.0*M_PI)));
        rad_vel_[servo_idx] = 0.0;
        sts_vel_[servo_idx] = 0;
        rad_eff_[servo_idx] = 0.0;
        sts_eff_[servo_idx] = 0;
    }
    void HardwareInterfaceST3215_cubic_leg1::read_controller_range(std::size_t ctrl_idx) {
    std::size_t servo_stard_idx = (ctrl_idx == 0) ? 0 : servo_N_/2;
    std::size_t servo_end_idx   = servo_stard_idx + servo_N_/2;
    for (std::size_t servo_idx = servo_stard_idx; servo_idx < servo_end_idx; servo_idx++) {
        uint8_t raw_data[6];
        if (sts_wb_[ctrl_idx].Read(servo_channels_[servo_idx], 56, raw_data, 6) == 6) {
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
PLUGINLIB_EXPORT_CLASS(cubic_leg1_namespace::HardwareInterfaceST3215_cubic_leg1, hardware_interface::SystemInterface)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
