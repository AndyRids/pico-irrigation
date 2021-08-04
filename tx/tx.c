#include <stdio.h>
#include <string.h>
#include "NRF24.h"

int main()
{
  uint8_t msg = 1;

  init_spi(); // Initialise SPI and GPIO pins

  init_nrf24(); // Initial config when device first powered

  init_nrf24_ptx_registers(PRX_ADDR_P3); // Config PTX specific registers

  sleep_ms(10000); // Sleep for 10s to facilitate opening PuTTy to read printf output

  debug_registers(); // printf register values

  set_mode(TX_MODE); // Activate TX_MODE

  // Enable IRQ for PIN_IRQ GPIO and set interrupt handler (irq_handler)
  gpio_set_irq_enabled_with_callback(PIN_IRQ, GPIO_IRQ_EDGE_FALL, true, &irq_handler);

  // Enable IRQ for PIN_BTN GPIO. Will use same interrupt handler (irq_handler)
  gpio_set_irq_enabled(PIN_BTN, GPIO_IRQ_EDGE_FALL, true);

  uint8_t value;

  while(true)
  {
    if (send_msg)
    {
      send_msg = false;

      sprintf(bufferOut, "12345");

      tx_message(bufferOut);

      printf("Tx message (%d): %s\n", msg, bufferOut);

      msg++;
    }
  }
}
