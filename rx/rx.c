#include <stdio.h>
#include <string.h>
#include "NRF24.h"

int main()
{
  init_spi(); // Initialise SPI and GPIO

  init_nrf24();

  init_nrf24_prx_registers();

  sleep_ms(10000);

  debug_registers();

  set_mode(RX_MODE);

  gpio_set_irq_enabled_with_callback(PIN_IRQ, GPIO_IRQ_EDGE_FALL, true, &nrf24_irq_handler);

  while (1);
}