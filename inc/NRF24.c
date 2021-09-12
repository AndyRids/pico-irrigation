#include "NRF24.h"

// LSB in 5 byte address array (PRX_ADDR_P0[LSB])
static const uint8_t LSB = 0; 

// Bytes in data buffer for read/write over SPI
static const size_t ONE_BYTE = 1;
static const size_t TWO_BYTES = 2;
static const size_t THREE_BYTES = 3; 
static const size_t FOUR_BYTES = 4;
static const size_t FIVE_BYTES = 5;

// Addresses for the 6 data pipes on the PRX device
uint8_t PRX_ADDR_P0[5] = {0x37, 0x37, 0x37, 0x37, 0x37};
uint8_t PRX_ADDR_P1[5] = {0xC7, 0xC7, 0xC7, 0xC7, 0xC7};
uint8_t PRX_ADDR_P2[5] = {0xC3, 0xC7, 0xC7, 0xC7, 0xC7};
uint8_t PRX_ADDR_P3[5] = {0xC4, 0xC7, 0xC7, 0xC7, 0xC7};
uint8_t PRX_ADDR_P4[5] = {0xC5, 0xC7, 0xC7, 0xC7, 0xC7};
uint8_t PRX_ADDR_P5[5] = {0xC6, 0xC7, 0xC7, 0xC7, 0xC7};

/**
 * 1. Initialise I/O for USB serial and all present stdio types.
 * 
 * 2. Initialise SPI0 at 5Mhz and set GPIO function for SCK,
 * MOSI and MISO pins as SPI (see GPIO_PINS.h file).
 * 
 * 3. Initialise CE, CSN and IRQ pins and set direction for
 * CE and CSN as GPIO_OUT and GPIO_IN for IRQ.
 * 
 * NOTE: IRQ pin on NRF24L01 is active-low. It is HIGH, before
 * being driven LOW on events such as Rx data or Tx data.
 */
void init_spi(void) {
  stdio_init_all(); // Initialise I/O for USB serial

  spi_init(SPI_PORT, 6000000); // Initialise SPI0 at 6MHz

  // Set GPIO function as SPI for SCK, MOSI & MISO
  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

  // Initialise CE, CSN & IRQ GPIO
  gpio_init(PIN_CE);
  gpio_init(PIN_CSN);
  gpio_init(PIN_IRQ);

  // Set direction for CE, CSN & IRQ GPIO
  gpio_set_dir(PIN_CE, GPIO_OUT);
  gpio_set_dir(PIN_CSN, GPIO_OUT);
  gpio_set_dir(PIN_IRQ, GPIO_IN);
}

/**
 * Drive CSN pin HIGH or LOW, depending on value.
 * HIGH and LOW are defined in NRF24 header file.
 * 
 * @param value HIGH (1) or LOW (0)
 */
void csn_put(uint8_t value) {
  gpio_put(PIN_CSN, value);
}

/**
 * Drive CE pin HIGH or LOW, depending on value.
 * HIGH and LOW are defined in NRF24 header file.
 * 
 * @param value HIGH (1) or LOW (0)
 */
void ce_put(uint8_t value) {
  gpio_put(PIN_CE, value);
}

/**
 * Drive CSN LOW, write data to the specified 
 * register through SPI and drive CSN HIGH.
 * 
 * W_REGISTER | (REGISTER_MASK & reg) ensures
 * last 3 MSB are 001 (W_REGISTER instruction) 
 * and that the first 5 LSB are the relevant 
 * register address (0x00 - 0x17).
 * 
 * e.g. To write to the CONFIG register (0x00):
 * 
 * spi_write_blocking(SPI_PORT, 0b00100000, ONE_BYTE);
 * spi_write_blocking(SPI_PORT, &buffer, ONE_BYTE);
 * 
 * @param reg Register address
 * @param buffer Data
 */
void w_register(uint8_t reg, uint8_t buffer) {
  reg = W_REGISTER | (REGISTER_MASK & reg);
  csn_put(LOW);
  spi_write_blocking(SPI_PORT, &reg, ONE_BYTE);
  spi_write_blocking(SPI_PORT, &buffer, ONE_BYTE);
  csn_put(HIGH);

  return;
}

/**
 * Drive CSN LOW, write address to the specified TX_ADDR 
 * or RX_ADDR_P0 - RX_ADDR_P5 registers through SPI and 
 * then drive CSN HIGH.
 * 
 * W_REGISTER | (REGISTER_MASK & reg) ensures
 * last 3 MSB are 001 (W_REGISTER instruction) 
 * and that the first 5 LSB are the relevant 
 * register address (0x00 - 0x17).
 * 
 * @param reg TX_ADDR / RX_ADDR_P0 - RX_ADDR_P5
 * @param buffer Data pipe or transmit address 
 * @param bytes Address width in bytes
 */
void w_address(uint8_t reg, uint8_t *buffer, uint8_t bytes) {
  csn_put(LOW);
  reg = W_REGISTER | (REGISTER_MASK & reg);
  spi_write_blocking(SPI_PORT, &reg, ONE_BYTE);
  spi_write_blocking(SPI_PORT, buffer, bytes);
  csn_put(HIGH);

  return;
}

/**
 * Drive CSN LOW, read data from address into the 
 * buffer, drive CSN HIGH and return the buffer.
 * 
 * Bitwise (REGISTER_MASK & reg) ensures last 3 MSB 
 * are 000 (R_REGISTER command) and first 5 LSB are 
 * the register address (0x00 - 0x17).
 * 
 * e.g. To read from the CONFIG register (0x00):
 * 
 * spi_write_blocking(SPI_PORT, 0b00000000, ONE_BYTE);
 * spi_read_blocking(SPI_PORT, NOP, &buffer, ONE_BYTE);
 * 
 * @param reg Register address
 * @return One byte of data read 
 */
uint8_t r_register(uint8_t reg) {
  // Byte of data read from register
  uint8_t buffer = 0;

  reg = (REGISTER_MASK & reg);

  csn_put(LOW);
  spi_write_blocking(SPI_PORT, &reg, ONE_BYTE);
  spi_read_blocking(SPI_PORT, NOP, &buffer, ONE_BYTE);
  csn_put(HIGH);

  // Return byte read from register
  return buffer;
}

/**
 * Drive CSN LOW, read data from address into the 
 * buffer, modifying the original buffer array and 
 * drive CSN HIGH.
 * 
 * NOTE: Works the same as r_register function, but 
 * some registers hold more than one byte, such as
 * RX_ADDR_P0 - RX_ADDR_P5 registers (5 bytes). An
 * array buffer is used to store the register value.
 * 
 * @param reg Register address
 * @param buffer array buffer to store data
 * @param bytes Number of bytes to read
 */
void r_register_all(uint8_t reg, uint8_t *buffer, uint8_t bytes) {

  reg = (REGISTER_MASK & reg);

  csn_put(LOW);
  spi_write_blocking(SPI_PORT, &reg, ONE_BYTE);
  spi_read_blocking(SPI_PORT, NOP, buffer, bytes);
  csn_put(HIGH);

  return;
}

/**
 * Drive CSN LOW, write FLUSH_TX or FLUSH_RX
 * instruction over SPI and drive CSN HIGH.
 * 
 * Flushes TX or RX FIFO.
 * 
 * @param buffer FLUSH_TX or FLUSH_RX
 */
void flush_buffer(uint8_t buffer) {
  if ((buffer != FLUSH_TX) | (buffer != FLUSH_RX)) return;
  
  csn_put(LOW);
  spi_write_blocking(SPI_PORT, &buffer, ONE_BYTE);
  csn_put(HIGH);

  return;
}

/**
 * Initial setup of NRF24L01, with power first supplied to 
 * RPi PICO and the NRF24L01 module.
 * 
 * 1. Wait 100ms for NRF24L01 to enter Power Down mode
 * 2. Drive CE pin LOW and CSN pin HIGH
 * 3. Config NRF24L01 registers
 * 
 * NOTE: The registers configured here are common to modules used
 * as both a primary transmitter (PTX) and primary receiver (PRX)
 * et PWR_UP (bit 1) in CONFIG register to power on NRF24L01. In 
 * this project the PRX would need all 6 data pipes enabled, in
 * order to comunicate with 6 PTX devices. 
 * 
 * This would be done through the EN_RXADDR register. By default 
 * this register only has data pipe 0 and data pipe 1 enabled. A 
 * PTX device will need its RX_ADDR_P0 and TX_ADDR set to one of 
 * the addresses stored in PRX RX_ADDR_P0 - RX_ADDR_P5 registers. 
 * 
 * If the PTX is not using auto acknowledge, only the PTX TX_ADDR 
 * register needs to be set with one of the addresses stored inside 
 * the PRX RX_ADDR_P0 - RX_ADDR_P5 registers.
 */
void init_nrf24(void) {
  /** With a VDD of 1.9V or higher, nRF24L01+ enters the Power on reset state **/

  sleep_ms(100); // nRF24L01+ enters Power Down mode after 100ms

  /** NRF24L01+ is now in Power Down mode. PWR_UP bit in the CONFIG register is low **/
  
  ce_put(LOW); // CE to low in preperation for entering Standby-I mode
  csn_put(HIGH); // CSN high in preperation for writing to registers

  /**
   * CONFIG register (0x00):
   * 
   * Mnemonic    | Bit | Set | Comment
   * (reserved)     7     0    Only '0' allowed
   * MASK_RX_DR     6     0    Mask interrupt caused by RX_DR
   * MASK_TX_DS     5     0    Mask interrupt caused by TX_DS
   * MASK_MAX_RT    4     0    Mask interrupt caused by MAX_RT
   * EN_CRC         3     1    Enable CRC
   * CRCO           2     1    CRC encoding scheme. 0: 1 byte, 1: 2 bytes
   * PWR_UP         1     1    1: Power up, 0: Power down
   * PRIM_RX        0     0    Rx & Tx control. 1: PRx, 0: PTx 
   * 
   * Value written to CONFIG register: 0b00001110
  **/
  w_register(CONFIG, 0b00001110); // PWR_UP bit is now high

  sleep_ms(2); // Crystal oscillator start up delay

  /** NRF24L01+ now in Standby-I mode. PWR_UP bit in CONFIG is high & CE pin is low **/

  /**
   * SETUP_AW register (0x03):
   * 
   * Setup Address Widths for all data pipes
   * 01: 3 bytes, 10: 4 bytes, 11: 5 bytes
   * 
   * Mnemonic    | Bit |  Set  | Comment
   * (reserved)   2:7   000000   Only '000000' allowed
   * AW           0:1    11      Rx or Tx Address field width 
   *
   * Value written to SETUP_AW register: 0b00000011
  **/
  w_register(SETUP_AW, 0b00000011);

  /**
   * SETUP_RETR register (0x04):
   * 
   * Setup of Automatic Retransmission.
   * 
   * ARD- 0000: Wait 250µS, 0010: Wait 500µS, 0100: Wait 750µS... etc.
   * ARC- 0000: Disabled, 0001: 1 retransmit ... 1111: 15 retransmit
   * 
   * Mnemonic    | Bit |  Set  | Comment
   * ARD          4:7    0100    Auto Retransmit Delay
   * ARC          0:3    1010    Auto Retransmit Count
   *
   * Value written to SETUP_RETR register: 0b01001010
  **/
  w_register(SETUP_RETR, 0b01001010); // ARC of 10 & ARD of 750µS

  /**
   * RF_CH register (0x05):
   * 
   * Set the nRF24L01+ frequency channel
   * 
   * Mnemonic    | Bit |   Set   | Comment
   * (reserved)     7      0       Only '0' allowed
   * RF_CH        0:6   1001100    Channel 2 - 127
   * 
   * Value written to RF_CH register: 0b01001100 (76)
  **/
  w_register(RF_CH, 0b01001100);

  /**
   * RF_SETUP register (0x06):
   * 
   * RF_DR_HIGH:- 00: 1Mbps, 01: 2Mbps, 10: 250kbps, 11: reserved
   * 
   * RF_PWR:- 00: -18dBm, 01: -12dBm, 10: -6dBm, 11: 0dBm
   * 
   * Mnemonic    | Bit | Set | Comment
   * CONT_WAVE      7     0    Enable continuous carrier transmit
   * (reserved)     6     0    Only '0' allowed
   * RF_DR_LOW      5     0    Set RF Data Rate to 250kbps
   * PLL_LOCK       4     0    Force PLL lock signal
   * RF_DR_HIGH     3     0    Select between the high speed data rates
   * RF_PWR        1:2   11    Set RF output power in TX mode
   * (obsolete)     0     0
   * 
   * Value written to RF_SETUP register: 0b00000110
  **/
  w_register(RF_SETUP, 0b00000110);

  /**
   * STATUS register (0x07):
   * 
   * RX_DR, TX_DS, MAX_RT:- Write 1 to clear bit
   * 
   * TX_FULL:- 1: Tx FIFO full, 0: Tx FIFO not full
   * 
   * RX_P_NO:- 000-101: Data Pipe, 111: Rx FIFO Empty
   * 
   * Mnemonic    | Bit | Set | Comment
   * (reserved)     7     0    Only '0' allowed
   * RX_DR          6     1    Data Ready RX FIFO interrupt
   * TX_DS          5     1    Data Sent TX FIFO interrupt
   * MAX_RT         4     1    Maximum Tx retransmits interrupt
   * RX_P_NO       1:3   111   Data pipe number for available payload 
   * TX_FULL        0     0    Tx FIFO full flag
   * 
   * Value written to RF_SETUP register: 0b01110000
  **/
  w_register(STATUS, 0b01110000); // Reset RX_DR, TX_DS & MAX_RT
}


/**
 * Setup registers relevant to a primary transmitter (PTX).
 * TX_ADDR must be the same as an address in primary receiver
 * (PRX) RX_ADDR_P0 - RX_ADDR_P5 registers, for PTX and PRX to
 * communicate. If using auto-acknowledge feature, the PTX must
 * have its RX_ADDR_P0 register set with the same address as the
 * TX_ADDR register.
 * 
 * @param address PRX_ADDR_P0 - PRX_ADDR_P5
 */
void init_nrf24_ptx_registers(uint8_t *address) {
  w_address(RX_ADDR_P0, (uint8_t*)address, FIVE_BYTES);
  w_address(TX_ADDR, (uint8_t*)address, FIVE_BYTES);

  /**
   * EN_AA register (0x01):
   * 
   * Enhanced ShockBurst Auto Acknowledgment (AA) setting
   * on data pipes 0 - 5:- 1: enable, 0: disable
   * 
   * NOTE: By default, AA is enabled for all data pipes.
   * A primary transmitter (PTX) only needs AA enabled
   * on data pipe 0. Disabling AA on data pipes 1 - 5
   * isn't necessary, but illustrates how to use and
   * write to the EN_AA register.
   * 
   * Mnemonic    | Bit | Set | Comment
   * (reserved)     7     0    Only '0' allowed
   * (reserved)     6     0    Only '0' allowed
   * ENAA_P5        5     0    Enable auto acknowledgement data pipe 5
   * ENAA_P4        4     0    Enable auto acknowledgement data pipe 4
   * ENAA_P3        3     0    Enable auto acknowledgement data pipe 3
   * ENAA_P2        2     0    Enable auto acknowledgement data pipe 2
   * ENAA_P1        1     0    Enable auto acknowledgement data pipe 1
   * ENAA_P0        0     1    Enable auto acknowledgement data pipe 0
   * 
   * Value written to EN_AA register: 0b00000001
  **/
  w_register(EN_AA, 0b00000001); // Disable AA for all pipes except pipe 0
}


/**
 * Setup registers relevant to a primary receiver (PRX).
 * 
 * 1. Enable all Rx address for all data pipes
 * 2. Set Rx payload width for all data pipes
 * 3. Set unique Rx address for all data pipes
 */
void init_nrf24_prx_registers(void) {

  /**
   * EN_RXADDR register (0x02):
   * 
   * Enabled Rx Addresses on data pipes 0 - 5.
   * 
   * ERX_P0 - ERX_P5:- 1: Enabled, 0: Disabled
   * 
   * Mnemonic    | Bit | Set | Comment
   * (reserved)    6:7   00    Only '00' allowed
   * ERX_P5         5     1    Enable data pipe 5
   * ERX_P4         4     1    Enable data pipe 4
   * ERX_P3         3     1    Enable data pipe 3
   * ERX_P2         2     1    Enable data pipe 2
   * ERX_P1         1     1    Enable data pipe 1
   * ERX_P0         0     1    Enable data pipe 0
   * 
   * Value written to EN_RXADDR register: 0b00111111
  **/
  w_register(EN_RXADDR, 0b00111111); // All Rx addresses enabled

  /**
   * RX_PW_P0 - RX_PW_P5 registers (0x11 - 0x16):
   * 
   * Set the number of bytes in the Rx payload for each data
   * pipe (0 - 5). Can be 1 - 32 bytes.
   * 
   * RX_PW_P0 - P5:- 0: Disabled, 00001: 1 byte... etc.
   * 
   * Mnemonic    | Bit |  Set  | Comment
   * (reserved)    6:7    00     Only '00' allowed
   * RX_P_NO       0:5  000101   Bytes in in data pipe RX payload 
   * 
   * Value written to RX_PW_P0 - RX_PW_P5 register: sizeof(payload_t)
   * 
  **/
  w_register(RX_PW_P0, sizeof(payload_t));
  w_register(RX_PW_P1, sizeof(payload_t));
  w_register(RX_PW_P2, sizeof(payload_t));
  w_register(RX_PW_P3, sizeof(payload_t));
  w_register(RX_PW_P4, sizeof(payload_t));
  w_register(RX_PW_P5, sizeof(payload_t));

  /**
   * A primary receiver (PRX) can receive data from
   * six primary transmitters (PTX), one per data pipe.
   * Each data pipe has a unique address.
   * 
   * 1. RX_ADDR_P0 and RX_ADDR_P1 can have a maximum of
   * a five byte address set. The size of the address is
   * set using the SETUP_AW register (0x03). This project
   * uses a five byte address width.
   * 
   * 2. RX_ADDR_P2 - RX_ADDR_P5 automatically share bits
   * 8 - 39 MSB of RX_ADDR_P1 and are simply set with a 
   * unique 1 byte value (LSB), which would act as bits 
   * 0 - 7 of the full 40 bit address.
  **/
 w_address(RX_ADDR_P0, PRX_ADDR_P0, FIVE_BYTES);
 w_address(RX_ADDR_P1, PRX_ADDR_P1, FIVE_BYTES);
 w_address(RX_ADDR_P2, &PRX_ADDR_P2[LSB], ONE_BYTE);
 w_address(RX_ADDR_P3, &PRX_ADDR_P3[LSB], ONE_BYTE);
 w_address(RX_ADDR_P4, &PRX_ADDR_P4[LSB], ONE_BYTE);
 w_address(RX_ADDR_P5, &PRX_ADDR_P5[LSB], ONE_BYTE);

  /**
   * EN_AA register (0x01):
   * 
   * Enhanced ShockBurst Auto Acknowledgment (AA) setting
   * on data pipes 0 - 5:- 1: enable, 0: disable
   * 
   * Mnemonic    | Bit | Set | Comment
   * (reserved)     7     0    Only '0' allowed
   * (reserved)     6     0    Only '0' allowed
   * ENAA_P5        5     1    Enable auto acknowledgement data pipe 5
   * ENAA_P4        4     1    Enable auto acknowledgement data pipe 4
   * ENAA_P3        3     1    Enable auto acknowledgement data pipe 3
   * ENAA_P2        2     1    Enable auto acknowledgement data pipe 2
   * ENAA_P1        1     1    Enable auto acknowledgement data pipe 1
   * ENAA_P0        0     1    Enable auto acknowledgement data pipe 0
   * 
   * Value written to EN_AA register: 0b00111111
  **/
  w_register(EN_AA, 0b00111111); // Enable AA for all data pipes

  return;
}


/**
 * Enables Enhanced ShockBurst™ auto acknowledgment (AA) 
 * function on the specified data pipes.
 * 
 * To enable AA on all data pipes, use EN_AA_ALL (0b00111111).
 * 
 * To disable AA, use EN_AA_NONE (0b00000000).
 * 
 * To enable AA on a specific pipe use ENAA_P0 - ENAA_P5. For
 * example, (1 << ENAA_P0) would enable AA on data pipe zero.
 * 
 * To enable AA on a range of pipes use ENAA_P0 - ENAA_P5. For 
 * example ((1 << ENAA_P0) | (1 << ENAA_P1) | (1 << ENAA_P1))
 * would enable AA on data pipes; zero, one, two and three.
 * 
 * en_auto_acknowledge(EN_AA_ALL);
 * en_auto_acknowledge(EN_AA_NONE);
 * en_auto_acknowledge(1 << ENAA_P0);
 * en_auto_acknowledge((1 << ENAA_P0) | (1 << ENAA_P1));
 * 
 * @param data_pipes EN_AA_ALL, EN_AA_NONE, (1 << ENAA_P0) | (1 << ENAA_P1)
 */
void en_auto_acknowledge(uint8_t data_pipes) {
  w_register(EN_AA, data_pipes);
}

/**
 * Puts the NRF24L01+ into Rx Mode fully or performs initial 
 * steps for Tx mode.
 * 
 * 1. Read CONFIG register into value and the current value
 * of PRIM_RX bit of CONFIG register into prim_rx.
 * 
 * NOTE: State diagram in the datasheet (6.1.1) highlights
 * conditions for entering Rx and Tx operating modes. One 
 * condition is the value of PRIM_RX (bit 0) in the CONFIG
 * register. PRIM_RX = 1 for Rx mode or 0 for Tx mode. CE
 * pin is driven HIGH for Rx mode and is only driven high 
 * in Tx mode to facilitate the Tx of data (10us+).
 * 
 * 2. If chosen mode is RX_MODE, value is modifed to set 
 * PRIM_RX (if not already) and is written to the CONFIG 
 * register. The RX FIFO is flushed, CE pin is driven HIGH 
 * and sleep timer for 130us as NRF24L01 enters Rx Mode.
 * 
 * 3. If chosen mode is TX_MODE, then value is modified to 
 * reset PRIM_RX (if not already) and is written to the 
 * CONFIG register. The TX FIFO is then flushed.
 * 
 * NOTE: State diagram indicates that the conditions for
 * Tx mode are; TX FIFO not empty, PRIM_RX = 0 and CE pin
 * is high for 10µs+. If TX FIFO is empty (it is), then
 * under these current conditions, NRF24L01 would leave 
 * standby-I mode and enter standby-II mode. In standby-II
 * mode, extra clock buffers are active and more current 
 * is used compared to standby-I mode (datasheet 6.1.3.2).
 * Tx mode will be entered fully when tx_message function
 * writes data to the TX FIFO, and drives the CE pin HIGH,
 * resulting in the data being transmitted.
 * 
 * RX_MODE and TX_MODE are defined in NRF24.h
 * 
 * @param mode RX_MODE (1) or TX_MODE (0)
 */
void set_mode(xcvr_mode_t transceiver_mode) {
  uint8_t value = 0; 
  uint8_t prim_rx = (value >> PRIM_RX) & 1; // value of PRIM_RX in CONFIG register

  value = r_register(CONFIG); // value of CONFIG register

  switch (transceiver_mode)
  {
    case RX_MODE:
      // if not already in RX_MODE
      if (prim_rx != RX_MODE)
      {
        value |= (1 << PRIM_RX); // Set PRIM_RX bit
        w_register(CONFIG, value); // Write new value to CONFIG register
      }

      flush_buffer(FLUSH_RX); // Flush RX FIFO

      /** NRF24L01+ still in Standby-I mode. PWR_UP bit in CONFIG is set & CE pin is LOW **/
      
      // Drive CE HIGH
      ce_put(HIGH);

      // NRF24L01+ enters Rx Mode after 130us
      sleep_us(130);

      /** NRF24L01+ now in Rx Mode. PRIM_RX bit in CONFIG is set (1) & CE pin is HIGH **/

    break;
    
    case TX_MODE:
      // Drive CE LOW
      ce_put(LOW);

      /** NRF24L01+ now in Standby-I mode. PWR_UP bit in CONFIG is set (1) & CE pin is LOW **/

      // if not already in TX_MODE
      if (prim_rx != TX_MODE)
      {
        value &= ~(1 << PRIM_RX); // reset PRIM_RX bit
        w_register(CONFIG, value); // Write new value to CONFIG register
      }

      flush_buffer(FLUSH_TX); // Flush TX FIFO

      /** NRF24L01+ enters Tx mode when TX FIFO is not empty, PRIM_RX = 0 & CE pin is HIGH for 10µs+ **/

    break;

    default:
      // TODO:
    break;
  }
}


/**
 * Write data over SPI for NR424L01 to Tx.
 * 
 * 1. Write W_TX_PAYLOAD instruction
 * 2. Write data contained in payload
 * 3. Drive CE pin HIGH to Tx data
 * 4. Drive CE pin LOW 
 * 
 * @param msg 
 */
void tx_message(payload_t* msg) {
  // W_TX_PAYLOAD instruction
  uint8_t cmd = W_TX_PAYLOAD;

  // Union allows access to payload_t data as an array
  spi_payload_t message;

  // Store payload_t msg data in payload_t payload in spi_payload_t message union
  message.payload = (*msg);
 
  csn_put(LOW);
  spi_write_blocking(SPI_PORT, &cmd, ONE_BYTE);
  spi_write_blocking(SPI_PORT, message.buffer, sizeof(message));
  csn_put(HIGH);

  ce_put(HIGH);
  sleep_us(100);
  ce_put(LOW);

  return;
}


/**
 * Read Rx data over SPI from NR424L01.
 * 
 * 1. Write R_RX_PAYLOAD instruction
 * 2. Read data into bufferIn (msg)
 * 
 * @param msg
 */
void rx_message(payload_prx_t* msg) {
  // Union allows access to payload_t data as an array
  spi_payload_t message;

  // Must read STATUS before reading payload from RX FIFO
  uint8_t status = r_register(STATUS);

  csn_put(LOW);
  // R_RX_PAYLOAD instruction
  uint8_t cmd = R_RX_PAYLOAD;
  spi_write_blocking(SPI_PORT, &cmd, ONE_BYTE);
  spi_read_blocking(SPI_PORT, NOP, message.buffer, sizeof(message));
  csn_put(HIGH);

  // Store ptx_id and moisture from PTX payload in payload_prx_t* msg
  (*msg).ptx_id = message.payload.ptx_id;
  (*msg).moisture = message.payload.moisture;

  // Store data pipe number (STATUS register bit 1:3) in payload_prx_t* msg
  (*msg).data_pipe = (status >> 1) & PIPE_MASK;

    // Reset RX_DR (bit 6) in STATUS register by writing 1
    w_register(STATUS, (1 << RX_DR));

  return;
}

uint8_t check_irq_bit(void) {
  // Value of the STATUS register
  uint8_t status = r_register(STATUS);

  // Test which interrupt was asserted
  uint8_t rx_dr = (status >> RX_DR) & 1; // Asserted when packet received
  uint8_t tx_ds = (status >> TX_DS) & 1; // Asserted when auto-acknowledge received
  uint8_t max_rt = (status >> MAX_RT) & 1; // Asserted when max retries reached

  if (rx_dr)
  { 
    // Indicate RX_DR bit asserted in STATUS register
    return RX_DR_ASSERTED; 
  }

  if (tx_ds)
  {
    // Reset MAX_RT (bit 4) in STATUS register by writing 1
    w_register(STATUS, (1 << TX_DS));
    // Indicate TX_DS bit asserted in STATUS register
    return TX_DS_ASSERTED; 
  }

  if (max_rt)
  {
    // Reset MAX_RT (bit 4) in STATUS register by writing 1
    w_register(STATUS, (1 << MAX_RT));
    // Indicate MAX_RT bit asserted in STATUS register
    return MAX_RT_ASSERTED; 
  }

  // Indicate no interrupt asserted in STATUS register
  return NONE_ASSERTED;
}

// typedef enum { RX_EMPTY, RX_FULL, TX_EMPTY = 4, TX_FULL } fifo_status_t;

uint8_t check_fifo_status(fifo_status_t bit_flag) {
  // Read value of FIFO_STATUS register
  uint8_t fifo_status = r_register(FIFO_STATUS);

  switch (bit_flag)
  {
    case RX_EMPTY:
      return (fifo_status >> RX_EMPTY) & 1;
    break;

    case RX_FULL:
      return (fifo_status >> RX_FULL) & 1;
    break;

    case TX_EMPTY:
      return (fifo_status >> TX_EMPTY) & 1;
    break;

    case TX_FULL:
      return (fifo_status >> TX_FULL) & 1;
    break;
    
    default:
      return 0;
    break;
  }
}

// Print register values for debugging
void debug_registers(void) {
  uint8_t value = 0;
  
  value = r_register(CONFIG);
  printf("CONFIG: 0x%X\n", value);

  value = r_register(EN_RXADDR);
  printf("EN_RXADDR: 0x%X\n", value);

  value = r_register(SETUP_AW);
  printf("SETUP_AW: 0x%X\n", value);

  value = r_register(RF_SETUP);
  printf("RF_SETUP: 0x%X\n", value);

  value = r_register(EN_AA);
  printf("EN_AA: 0x%X\n\n", value);

  return;
}

void debug_rx_address_pipes(uint8_t reg) {
  uint8_t buffer[5];

  switch (reg)
  {
    case RX_ADDR_P0:
      r_register_all(RX_ADDR_P0, buffer, FIVE_BYTES);
      printf("RX_ADDR_P0: 0x%X 0x%X 0x%X 0x%X 0x%X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
    break;

    case RX_ADDR_P1:
      r_register_all(RX_ADDR_P1, buffer, FIVE_BYTES);
      printf("RX_ADDR_P1: 0x%X 0x%X 0x%X 0x%X 0x%X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
    break;

    case TX_ADDR:
      r_register_all(RX_ADDR_P1, buffer, FIVE_BYTES);
      printf("TX_ADDR: 0x%X 0x%X 0x%X 0x%X 0x%X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
    break;
    
    default:
      buffer[LSB] = r_register(reg);
      printf("RX_ADDR_P%d: 0x%X\n", reg - 10, buffer[LSB]);
    break;
  }
}