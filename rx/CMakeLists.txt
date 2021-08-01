include_directories(../inc/)

add_executable(rx rx.c)

# target_include_directories(rx PUBLIC inc)
# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(rx pico_stdlib hardware_spi NRF24)

pico_enable_stdio_usb(rx 1)
pico_enable_stdio_uart(rx 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(rx)
