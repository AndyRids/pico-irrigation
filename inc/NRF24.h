#ifndef NRF24
#define NRF24

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "NRF24L01.h"
#include "GPIO_PINS.h"

static const uint8_t LSB; // LSB in 5 byte address array (PRX_ADDR_P0[LSB])

static const size_t ONE_BYTE; // Data buffer size for SPI read/write 
static const size_t TWO_BYTES; // Data buffer size for SPI read/write 
static const size_t THREE_BYTES; // Data buffer size for SPI read/write 
static const size_t FOUR_BYTES; // Data buffer size for SPI read/write 
static const size_t FIVE_BYTES; // Data buffer size for SPI read/write 

// Set transceiver (XCVR) mode
typedef enum { TX_MODE, RX_MODE } xcvr_mode_t;

// Individual PTX ID, which can correspond to the relevant PRX data pipe
typedef enum { PTX_0, PTX_1, PTX_2, PTX_3, PTX_4, PTX_5 } ptx_id_t;

// FIFO_STATUS register TX & RX FIFO full/empty flags
typedef enum { RX_EMPTY, RX_FULL, TX_EMPTY = 4, TX_FULL } fifo_status_t;

// Interrupt bit asserted in STATUS register
typedef enum { NONE_ASSERTED, RX_DR_ASSERTED, TX_DS_ASSERTED, MAX_RT_ASSERTED } asserted_bit_t;

/**
 * Used to format message data passed between each PTX
 * and the PRX though the tx_message and rx_message functions.
 */
typedef struct
{ 
  ptx_id_t ptx_id;
  uint8_t moisture : 8;
} payload_t;


/**
 * The PRX STATUS register bits 1:3 specify the data pipe number 
 * the payload was was received on. payload_prx_t stores this data 
 * pipe number.
 */
typedef struct
{
  ptx_id_t ptx_id : 8;
  uint8_t data_pipe : 8;
  uint8_t moisture : 8;
} payload_prx_t;

/**
 * Used to store a payload_t argument for the tx_message and rx_message 
 * functions. The union allows access to bytes within the payload_t in 
 * the form of an array. Data received by the PRX can be read over SPI 
 * into this buffer using spi_read_blocking. Data for transmission by 
 * a PTX can be written over SPI using the spi_write_blocking function.
 */
typedef union
{
  payload_t payload;
  uint8_t buffer[sizeof(payload_t)];
} spi_payload_t;

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
void init_nrf24(void);

// Config PTX specific registers
void init_nrf24_ptx_registers(uint8_t *address);

// Config PRX specific registers
void init_nrf24_prx_registers(void);

// Data pipes to enable auto-acknowledge
void en_auto_acknowledge(uint8_t data_pipes);

// Activate RX_MODE or TX_MODE
void set_mode(xcvr_mode_t mode);

// Tx data
void tx_message(payload_t *msg);

// Rx data
void rx_message(payload_prx_t *msg);

// check which interrupt bit is asserted in STATUS register
uint8_t check_irq_bit(void);

// Check if RX FIFO is empty
uint8_t check_fifo_status(fifo_status_t bit_flag);

// printf register values
void debug_registers(void);

// printf RX_ADDR_P0 - RX_ADDR_P5 & TX_ADDR register values
void debug_rx_address_pipes(uint8_t reg);

#endif