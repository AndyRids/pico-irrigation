#include <stdio.h>
#include <string.h>
#include "NRF24.h"

int main()
{
  init_spi(); // Initialise SPI and GPIO pins

  init_nrf24(); // Initial config when device first powered

  init_nrf24_prx_registers(); // Config PRX specific registers

  sleep_ms(10000); // Sleep for 10s to facilitate opening PuTTy to read printf output

  debug_registers(); // printf register values

  set_mode(RX_MODE); // Activate RX_MODE

  // IRQ interrupt handler
  gpio_set_irq_enabled_with_callback(PIN_IRQ, GPIO_IRQ_EDGE_FALL, true, &irq_handler);

  // Infinite loop
  while (true);
}