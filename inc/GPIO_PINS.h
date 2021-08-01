#ifndef GPIO_PINS
#define GPIO_PINS

#define SPI_PORT spi0 // Hardware SPI instance

#define PIN_SCK   2 // SPI Clock
#define PIN_MOSI  3 // SPI Data Input
#define PIN_MISO  4 // SPI Data Output
#define PIN_CSN   5 // SPI Chip Select
#define PIN_CE    6 // Chip Enable activates Rx or Tx mode
#define PIN_IRQ   0 // IRQ (active-low)
#define PIN_BTN   20 // Push button GPIO

#define HIGH  1
#define LOW   0

#endif