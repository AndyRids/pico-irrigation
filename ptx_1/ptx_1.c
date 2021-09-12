#include <stdio.h>
#include <string.h>
#include "pico/util/queue.h"
#include "NRF24.h"
#include "SEN0308.h"

typedef struct { void *func; } queue_entry_t;

static queue_t call_queue;

  // send mssage flag
static volatile bool send_msg = true;

 /**
   * IRQ handler function for the active-low IRQ pin
   * on the NRF24L01. Acts as the callback function
   * for the pick-sdk gpio_set_irq_enabled_with_callback 
   * function.
   * 
   * Adds a pointer for the check_irq_bit function to
   * the call_queue, which is monitored by the main loop.
   * 
   * check_irq_bit reads the STATUS register and returns
   * a value (asserted_bit_t), which coresponds to the 
   * type of interrupt; packet received, 
   * 
  **/
void gpio_irq_handler() {
  queue_entry_t entry = {&check_irq_bit};

  queue_add_blocking(&call_queue, &entry);
}

int main()
{
  // Tx message counter
  uint8_t msg = 1;

  // Tx payload format; PTX id, PRX data pipe, soil moisture %
  payload_t payload_tx = { PTX_0, 0 };

  init_spi(); // Initialise SPI and GPIO pins

  init_adc(); // Initialise ADC and GPIO pins

  init_nrf24(); // Initial config when device first powered

  // Config PTX specific registers and Tx payloads to PRX data pipe 0
  init_nrf24_ptx_registers(PRX_ADDR_P5); 

  set_mode(TX_MODE); // Activate TX_MODE

  sleep_ms(10000); // Sleep for 10s to facilitate opening PuTTy to read printf output

  debug_registers(); // printf register values
  debug_rx_address_pipes(RX_ADDR_P0); // printf RX_ADDR_P0 register
  debug_rx_address_pipes(TX_ADDR); // printf TX_ADDR register

  // Initialise the call_queue utilized by gpio_irq_handler
  queue_init(&call_queue, sizeof(queue_entry_t), 6);

  // Enable IRQ for PIN_IRQ GPIO and set interrupt handler (irq_handler)
  gpio_set_irq_enabled_with_callback(PIN_IRQ, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);


  while (true)
  {
    if (send_msg)
    {
      send_msg = false; // Reset send message flag

      // Read moisture value into payload
      payload_tx.moisture = read_moisture();

      // Transmit the payload to the PRX
      tx_message(&payload_tx);

      printf("Tx message #%d: %d%% moisture\n", msg, payload_tx.moisture);

      // Tranmission count for debugging
      msg++;
    }

    if (!queue_is_empty(&call_queue))
    {
      queue_entry_t entry;

      queue_remove_blocking(&call_queue, &entry);

      uint8_t (*func)() = (uint8_t(*)())(entry.func);

      uint8_t irq_bit = (*func)();

      switch (irq_bit)
      {       
        case RX_DR_ASSERTED:
          // Not possible in this PTX example as never enters Rx mode
        break;
        
        case TX_DS_ASSERTED:
          printf("Auto-acknowledge received\n");
          sleep_ms(20000);
          send_msg = true;
        break;

        case MAX_RT_ASSERTED:
          printf("Max retries reached\n");
        break;

        default:
          printf("irq_bit: %d\n", irq_bit);
        break;
      }
    }
  }
  return 0;  
}