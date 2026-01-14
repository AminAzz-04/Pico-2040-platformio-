cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_project C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

add_executable(pico_project
        src/main.cpp
        src/tb6612.cpp
        src/pid.cpp
        src/qtr_rc.cpp
)

# så #include "tb6612.hpp" funker osv
target_include_directories(pico_project PRIVATE src)

target_link_libraries(pico_project
        pico_stdlib
        hardware_pwm
        hardware_timer
        hardware_gpio
)

pico_enable_stdio_usb(pico_project 1)
pico_enable_stdio_uart(pico_project 0)

pico_add_extra_outputs(pico_project)
