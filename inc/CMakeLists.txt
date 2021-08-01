add_library(NRF24 STATIC NRF24.c)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(NRF24 pico_stdlib hardware_spi)
