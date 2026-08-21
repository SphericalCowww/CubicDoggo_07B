cmake_minimum_required(VERSION 3.8)
project(my_toolbox_scs_workbench)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)

add_subdirectory(SCServo_Linux/SCServo_Linux_220329/SCServo_Linux)

###########################################################################
add_executable(scanUpdateID_zeroing src/scanUpdateID_zeroing.cpp)
target_include_directories(scanUpdateID_zeroing PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SCServo_Linux/SCServo_Linux_220329/SCServo_Linux>
)
ament_target_dependencies(scanUpdateID_zeroing rclcpp rclcpp_lifecycle hardware_interface)
target_link_libraries(scanUpdateID_zeroing SCServo)

add_executable(testSpin_servo1 src/testSpin_servo1.cpp)
target_include_directories(testSpin_servo1 PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SCServo_Linux/SCServo_Linux_220329/SCServo_Linux>
)
ament_target_dependencies(testSpin_servo1 rclcpp rclcpp_lifecycle hardware_interface)
target_link_libraries(testSpin_servo1 SCServo)

add_executable(testSpin_servo2_controller2 src/testSpin_servo2_controller2.cpp)
target_include_directories(testSpin_servo2_controller2 PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SCServo_Linux/SCServo_Linux_220329/SCServo_Linux>
)
ament_target_dependencies(testSpin_servo2_controller2 rclcpp rclcpp_lifecycle hardware_interface)
target_link_libraries(testSpin_servo2_controller2 SCServo)

install(TARGETS scanUpdateID_zeroing testSpin_servo1 testSpin_servo2_controller2
    DESTINATION lib/${PROJECT_NAME}
)

ament_package()




