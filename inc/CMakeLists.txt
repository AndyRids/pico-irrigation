# Add NRF24 & SEN0308 libraries to the project
add_library(NRF24 STATIC NRF24.c)
add_library(SEN0308 STATIC SEN0308.c)

# Link pico_stdlib & hardware_spi with NRF24 library
target_link_libraries(NRF24 pico_stdlib pico_multicore hardware_spi)

# Link pico_stdlib & hardware_adc with SEN0308 library
target_link_libraries(SEN0308 pico_stdlib hardware_adc)
