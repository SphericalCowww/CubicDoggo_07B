cmake_minimum_required(VERSION 3.8)
project(SCServo)

file(GLOB SRCS "*.cpp")
file(GLOB HDRS "*.h")

add_library(SCServo STATIC ${SRCS} ${HDRS})

target_compile_options(SCServo PRIVATE -Wno-vla)
target_include_directories(SCServo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SCServo pthread)
