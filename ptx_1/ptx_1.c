#include <stdio.h>
#include <string.h>
#include <math.h>
#include "NRF24.h"
#include "SEN0308.h"

int main()
{
  // Tx message counter
  uint8_t msg = 1;
  uint8_t moisture;

  init_spi(); // Initialise SPI and GPIO pins

  init_adc(); // Initialise ADC and GPIO pins

  init_nrf24(); // Initial config when device first powered

  init_nrf24_ptx_registers(PRX_ADDR_P0); // Config PTX specific registers

  sleep_ms(10000); // Sleep for 10s to facilitate opening PuTTy to read printf output

  debug_registers(); // printf register values

  set_mode(TX_MODE); // Activate TX_MODE

  // Enable IRQ for PIN_IRQ GPIO and set interrupt handler (irq_handler)
  gpio_set_irq_enabled_with_callback(PIN_IRQ, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  // Enable IRQ for PIN_BTN GPIO. Will use same interrupt handler (irq_handler)
  gpio_set_irq_enabled(PIN_BTN, GPIO_IRQ_EDGE_FALL, true);

  while (true)
  {
    moisture = read_moisture();

    sprintf(bufferOut, "PTX-1 %d", moisture);

    tx_message(bufferOut);

    printf("Tx message (%d): %s\n", msg, bufferOut);

    msg++;
  }


/*   while(true)
  {
    // send_msg is set to true in irq_handler, if the GPIO that triggered the interrupt is PIN_BTN
    if (send_msg)
    {
      // Reset send_msg flag
      send_msg = false;

      // Put test string variable into bufferOut
      sprintf(bufferOut, "12345");

      // Tx bufferOut
      tx_message(bufferOut);

      printf("Tx message (%d): %s\n", msg, bufferOut);

      // Tx message counter for debugging/testing
      msg++;
    }
  } */
}