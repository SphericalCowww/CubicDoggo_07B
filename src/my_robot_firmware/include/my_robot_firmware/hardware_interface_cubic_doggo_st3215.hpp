#ifndef HARDWARE_INTERFACE_CUBIC_DOGGO_ST3215_HPP
#define HARDWARE_INTERFACE_CUBIC_DOGGO_ST3215_HPP

#include "hardware_interface/system_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "SCServo.h"

#define MIN_POSITION 0
#define MAX_POSITION 4095
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cubic_doggo_namespace {
    class HardwareInterfaceST3215_cubic_doggo: public hardware_interface::SystemInterface
    {
        public:
            hardware_interface::CallbackReturn 
                on_init(const hardware_interface::HardwareComponentInterfaceParams &params) override;
            hardware_interface::return_type 
                read(const rclcpp::Time & time, const rclcpp::Duration &period) override;
            hardware_interface::return_type 
                write(const rclcpp::Time & time, const rclcpp::Duration &period) override;
            
            hardware_interface::CallbackReturn 
                on_configure(const rclcpp_lifecycle::State &previous_state) override;
            hardware_interface::CallbackReturn 
                on_activate(const rclcpp_lifecycle::State &previous_state) override;
            hardware_interface::CallbackReturn 
                on_deactivate(const rclcpp_lifecycle::State &previous_state) override;
            virtual ~HardwareInterfaceST3215_cubic_doggo();
        private:
            std::string port_name_front_;
            std::string port_name_back_;
            int         baud_rate_;

            SMS_STS sts_wb_[2];
            std::size_t sts_idx_ = 0;

            bool write_first_call_ = true;
            rclcpp::Time start_time_;

            std::vector<std::string> joint_names;
            std::size_t servo_N_ = 12;
            uint8_t servo_channels_[12];
            double  rad_pos_init_  [12];
            double  rad_pos_       [12];
            double  rad_vel_       [12];
            double  rad_acc_       [12];
            double  rad_eff_       [12];
            s16     sts_pos_       [12];
            u16     sts_vel_       [12];
            u8      sts_acc_       [12];
            s16     sts_eff_       [12];

            void initialize_servo_(uint8_t servo_idx);
            void read_controller_range(std::size_t ctrl_idx);
    };    
}

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

