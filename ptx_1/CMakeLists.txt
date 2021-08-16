include_directories(../inc/)

add_executable(ptx_1 ptx_1.c)

# target_include_directories(tx PUBLIC inc)
# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(ptx_1 pico_stdlib NRF24 SEN0308)

pico_enable_stdio_usb(ptx_1 1)
pico_enable_stdio_uart(ptx_1 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ptx_1)