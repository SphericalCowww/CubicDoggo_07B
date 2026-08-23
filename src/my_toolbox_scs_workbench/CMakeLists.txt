cmake_minimum_required(VERSION 3.8)
project(my_toolbox_scs_workbench)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)

# publish SCS library
add_subdirectory(SCServo_Linux/SCServo_Linux_220329/SCServo_Linux)
set_target_properties(SCServo PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")
install(
  DIRECTORY SCServo_Linux/SCServo_Linux_220329/SCServo_Linux/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h"
)
install(TARGETS SCServo
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)
ament_export_libraries(SCServo)
ament_export_include_directories(include)
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

add_executable(testSpin_servo3 src/testSpin_servo3.cpp)
target_include_directories(testSpin_servo3 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SCServo_Linux/SCServo_Linux_220329/SCServo_Linux>
)
ament_target_dependencies(testSpin_servo3 rclcpp rclcpp_lifecycle hardware_interface)
target_link_libraries(testSpin_servo3 SCServo)

install(TARGETS scanUpdateID_zeroing testSpin_servo1 testSpin_servo2_controller2 testSpin_servo3
    DESTINATION lib/${PROJECT_NAME}
)

ament_package()




