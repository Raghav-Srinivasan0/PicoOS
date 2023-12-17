cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(os C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(os
  kernel.c
)
pico_enable_stdio_usb(os 1)
pico_enable_stdio_uart(os 1)
pico_add_extra_outputs(os)
target_link_libraries(os pico_stdlib hardware_flash hardware_sync hardware_gpio)