#include <stdio.h>
#include <string.h>
#include "pico/util/queue.h"
#include "NRF24.h"

typedef struct { void *func; } queue_entry_t;

static queue_t call_queue;

void gpio_irq_handler() {

  queue_entry_t entry = {&check_irq_bit};

  queue_add_blocking(&call_queue, &entry);
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
  // Turn off motor
  gpio_put(PIN_MTR, LOW);

  // Value in μs to fire in the future
  return 0;
}


int main(void)
{
  payload_prx_t payload_rx;

  init_spi(); // Initialise SPI and GPIO pins

  gpio_init(PIN_MTR);
  gpio_set_dir(PIN_MTR, GPIO_OUT);

  init_nrf24(); // Initial config when device first powered

  init_nrf24_prx_registers(); // Config PRX specific registers

  set_mode(RX_MODE); // Activate RX_MODE

  sleep_ms(10000); // Sleep for 10s to facilitate opening PuTTy to read printf output

  debug_registers(); // printf register values
  debug_rx_address_pipes(RX_ADDR_P0); // printf RX_ADDR_P0 register
  debug_rx_address_pipes(RX_ADDR_P1); // printf RX_ADDR_P1 register
  debug_rx_address_pipes(RX_ADDR_P2); // printf RX_ADDR_P2 register
  debug_rx_address_pipes(RX_ADDR_P3); // printf RX_ADDR_P3 register
  debug_rx_address_pipes(RX_ADDR_P4); // printf RX_ADDR_P4 register
  debug_rx_address_pipes(RX_ADDR_P5); // printf RX_ADDR_P5 register

  queue_init(&call_queue, sizeof(queue_entry_t), 6);

  // IRQ interrupt handler
  gpio_set_irq_enabled_with_callback(PIN_IRQ, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  // Infinite loop
  while (true)
  {
    if (!queue_is_empty(&call_queue))
    {
      queue_entry_t entry;

      queue_remove_blocking(&call_queue, &entry);

      uint8_t (*func)() = (uint8_t(*)())(entry.func);

      uint8_t irq_bit = (*func)();

      // printf("irq_bit: %d\n", irq_bit);

      switch (irq_bit)
      {      
        case RX_DR_ASSERTED:

          do
          {
            rx_message(&payload_rx);

            printf("Rx Message - PTX ID: %d, Data Pipe: %d, Moisture: %d\n", payload_rx.ptx_id, payload_rx.data_pipe, payload_rx.moisture);
            
            if (payload_rx.moisture < 60 && !gpio_get(PIN_MTR))
            {
              printf("Switching PIN_MTR on");
              gpio_put(PIN_MTR, HIGH); // Power motor
              add_alarm_in_ms(10000, alarm_callback, NULL, false); // Add alarm to turn off motor in 10 seconds
            }
          } while (!check_fifo_status(RX_EMPTY));
          
        break;
        
        case TX_DS_ASSERTED:
          // Not used on PRX
        break;

        case MAX_RT_ASSERTED:
          // Not used on PRX
        break;

        default:

        break;
      }
    }
  }
  return 0;
}