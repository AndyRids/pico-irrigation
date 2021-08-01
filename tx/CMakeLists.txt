include_directories(../inc/)

add_executable(tx tx.c)

# target_include_directories(tx PUBLIC inc)
# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(tx pico_stdlib hardware_spi NRF24)

pico_enable_stdio_usb(tx 1)
pico_enable_stdio_uart(tx 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(tx)