#ifndef GPIO_PINS
#define GPIO_PINS

#define SPI_PORT  spi0 // Hardware SPI instance
#define ADC_INPUT 1 // ADC input 1 (GPIO 27)

#define PIN_SCK   2 // SPI Clock
#define PIN_MOSI  3 // SPI Data Input
#define PIN_MISO  4 // SPI Data Output
#define PIN_CSN   5 // SPI Chip Select
#define PIN_CE    6 // Chip Enable activates Rx or Tx mode
#define PIN_IRQ   7 // IRQ (active-low)
#define PIN_BTN   20 // Push button GPIO
#define PIN_MTR   22 // Motorized ball valve GPIO
#define PIN_ADC   27 // ADC1
#define PIN_DFR   26 // Power on DFRobot moisture sensor 

#define HIGH  1
#define LOW   0

#endif