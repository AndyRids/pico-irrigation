include_directories(../inc/)

add_executable(prx prx.c)

# target_include_directories(rx PUBLIC inc)
# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(prx pico_multicore pico_stdlib NRF24)

pico_enable_stdio_usb(prx 1)
pico_enable_stdio_uart(prx 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(prx)
