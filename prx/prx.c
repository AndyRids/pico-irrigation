#include <stdio.h>
#include <string.h>
#include "hardware/irq.h"
#include "NRF24.h"


int64_t alarm_callback(alarm_id_t id, void *user_data) {
  timer_end = true;

  // Value in us to fire in the future
  return 0;
}

void core1_irq_handler() {
  uint8_t ms = multicore_fifo_pop_blocking();

  add_alarm_in_ms(ms, alarm_callback, NULL, false);

  motor_on = true;

  gpio_put(PIN_MTR, HIGH);
  
  while (!timer_end)
  {
    tight_loop_contents();
  }

  motor_on = false;

  gpio_put(PIN_MTR, LOW);

  multicore_fifo_clear_irq();
}


void core1_entry() {

  multicore_fifo_clear_irq();

  irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_irq_handler);

  irq_set_enabled(SIO_IRQ_PROC1, true);


  while(true)
  {
    // Empty function intended to be called by any tight hardware polling loop
    tight_loop_contents();
  };
}


int main()
{
  init_spi(); // Initialise SPI and GPIO pins

  init_nrf24(); // Initial config when device first powered

  init_nrf24_prx_registers(); // Config PRX specific registers

  sleep_ms(10000); // Sleep for 10s to facilitate opening PuTTy to read printf output

  debug_registers(); // printf register values

  set_mode(RX_MODE); // Activate RX_MODE

  // Launch Pico core1
  multicore_launch_core1(core1_entry);

  // IRQ interrupt handler
  gpio_set_irq_enabled_with_callback(PIN_IRQ, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  // Infinite loop
  while (true)
  {
    // Empty function intended to be called by any tight hardware polling loop
    tight_loop_contents();
  };
}