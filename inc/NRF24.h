#ifndef NRF24
#define NRF24

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "NRF24L01.h"
#include "GPIO_PINS.h"

// For readability in register functions
#define ONE_BYTE    1 
#define THREE_BYTES 3
#define FIVE_BYTES  5

#define ADDR_WIDTH    3
#define PAYLOAD_WIDTH 9

#define LSB 0 // LSB in 5 byte address array (PRX_ADDR_P0[LSB])

#define RX_MODE 1 // Used to set RX Mode
#define TX_MODE 0 // Used to set TX Mode

extern volatile bool send_msg; // Tx message flag
extern volatile bool motor_on; // Motorized ball valve flag 
extern volatile bool timer_end; // Alarm callback flag

extern char bufferIn[9]; // Store Rx data
extern char bufferOut[9]; // Store data for Tx

extern uint8_t PRX_ADDR_P0[5]; // PRX receive address for data pipe 0
extern uint8_t PRX_ADDR_P1[5]; // PRX receive address for data pipe 1
extern uint8_t PRX_ADDR_P2[5]; // PRX receive address for data pipe 2
extern uint8_t PRX_ADDR_P3[5]; // PRX receive address for data pipe 3
extern uint8_t PRX_ADDR_P4[5]; // PRX receive address for data pipe 4
extern uint8_t PRX_ADDR_P5[5]; // PRX receive address for data pipe 5

// Initialise SPI and GPIO pins
void init_spi();

// Drive CSN pin HIGH or LOW
void csn_put(uint8_t value);

// Drive CE pin HIGH or LOW
void ce_put(uint8_t value);

// Write to a register
void w_register(uint8_t reg, uint8_t buffer);

// Write an address to RX_ADDR_P0 - RX_ADDR_P5 registers
void w_address(uint8_t reg, uint8_t *buffer, uint8_t bytes);

// Read one byte from a register
uint8_t r_register(uint8_t reg);

// Read more than one byte from a register
void r_register_all(uint8_t reg, uint8_t *buffer, uint8_t bytes);

// Flush either Rx or Tx FIFO
void flush_buffer(uint8_t buffer);

// Initial config when device first powered
void init_nrf24();

// Config PTX specific registers
void init_nrf24_ptx_registers();

// Config PRx specific registers
void init_nrf24_prx_registers();

// Activate RX_MODE or TX_MODE
void set_mode(uint8_t mode);

// Tx data
void tx_message(char *msg);

// Rx data
void rx_message(char *msg);

// Check Rx FIFO for new data
uint8_t is_message();

// IRQ interrupt handler
void gpio_irq_handler();

// printf register values
void debug_registers();

#endif