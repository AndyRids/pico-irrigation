# Add NRF24 & SEN0308 libraries to the project
add_library(NRF24 INTERFACE)
add_library(SEN0308 INTERFACE)

target_sources(NRF24 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/NRF24.c)
target_sources(SEN0308 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/SEN0308.c)

# Link pico_stdlib & hardware_spi with NRF24 library
target_link_libraries(NRF24 INTERFACE pico_stdlib hardware_spi)

# Link pico_stdlib & hardware_adc with SEN0308 library
target_link_libraries(SEN0308 INTERFACE pico_stdlib hardware_adc)
