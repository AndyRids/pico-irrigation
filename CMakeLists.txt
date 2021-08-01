cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(sensor C CXX ASM)

pico_sdk_init()

# include_directories(inc)

add_subdirectory(inc)
add_subdirectory(rx)
add_subdirectory(tx)