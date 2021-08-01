#include <stdio.h>
#include <string.h>
#include "NRF24.h"

int main()
{
  
  char bufferOut[5];
  uint8_t msg = 1;

  init_spi(); // Initialise SPI and GPIO

  init_nrf24();

  init_nrf24_ptx_registers(PRX_ADDR_P0);

  sleep_ms(10000);

  debug_registers();

  set_mode(TX_MODE);

  gpio_set_irq_enabled_with_callback(PIN_BTN, GPIO_IRQ_EDGE_RISE, true, &button_irq_handler);

  while(true)
  {
    if (send_msg)
    {
      send_msg = false;

      sprintf(bufferOut, "12345");

      tx_message(bufferOut);

      printf("Tx message (%d): %s\n", msg, bufferOut);

      msg++;

      sleep_ms(1000);
    }
  }
}
