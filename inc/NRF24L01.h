#ifndef NRF24L01
#define NRF24L01

/**
 * Nordic Semiconductor NRF24L01+
 * 
 * The nRF24L01+ is a single chip 2.4GHz transceiver with an embedded baseband protocol
 * engine (Enhanced ShockBurst™), suitable for ultra low power wireless applications.
 * The nRF24L01+ is designed for operation in the world wide ISM frequency band at 
 * 2.400 - 2.4835GHz.  
 * 
 * Header file contains the full register map for the NRF24L01 and defines specific
 * register bit mnemonics that are useful for interfacing with the NRF24L01 over SPI.
 */


// SPI commands [8.3.1 in Datasheet]
#define R_REGISTER    0x00 // Read register [000] + [5 bit register address]
#define W_REGISTER    0x20 // Write register [001] + [5 bit register address]
#define R_RX_PAYLOAD  0x61 // Read Rx payload (0b01100001)
#define W_TX_PAYLOAD  0xA0 // Write Tx payload (0b10100000)
#define FLUSH_TX      0xE1 // Flush Tx FIFO (Tx mode)
#define FLUSH_RX      0xE2 // Flush Rx FIFO (Rx mode)
#define REUSE_TX_PL   0xE3
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define NOP           0xFF // No Operation

// Register map [9 in Datasheet]
#define CONFIG      0x00 // Configuration register
#define EN_AA       0x01 // Enhanced ShockBurst
#define EN_RXADDR   0x02 // Enabled Rx addresses
#define SETUP_AW    0x03 // Data pipe address width
#define SETUP_RETR  0x04 // Setup automatic retransmission
#define RF_CH       0x05 // RF Channel
#define RF_SETUP    0x06 // RF Setup register
#define STATUS      0x07 // Status register
#define OBSERVE_TX  0x08 // Tx observe register
#define RPD         0x09 // Received Power Detector (NRF24L01+)

#define RX_ADDR_P0 0x0A // Rx address data pipe 0
#define RX_ADDR_P1 0x0B // Rx address data pipe 1
#define RX_ADDR_P2 0x0C // Rx address data pipe 2
#define RX_ADDR_P3 0x0D // Rx address data pipe 3
#define RX_ADDR_P4 0x0E // Rx address data pipe 4
#define RX_ADDR_P5 0x0F // Rx address data pipe 5

#define TX_ADDR 0x10 // Tx address (PTx)

#define RX_PW_P0 0x11 // Bytes in Rx payload in data pipe 0
#define RX_PW_P1 0x12 // Bytes in Rx payload in data pipe 1
#define RX_PW_P2 0x13 // Bytes in Rx payload in data pipe 2
#define RX_PW_P3 0x14 // Bytes in Rx payload in data pipe 3
#define RX_PW_P4 0x15 // Bytes in Rx payload in data pipe 4
#define RX_PW_P5 0x16 // Bytes in Rx payload in data pipe 5

#define FIFO_STATUS 0x17 // FIFO Status register
#define DYNPD       0x1C // Enable dynamic payload length

// Bitwise masks
#define REGISTER_MASK 0x1F // 0b00011111
#define FIFO_MASK     0x01 // 0b00000001

/**
 * CONFIG register PWR_UP & PRIM_RX:
 * 
 * PWR_UP (bit 1) is set (1) to power up 
 * the device or reset (0) to power down. 
 * 
 * PRIM_RX (bit 0) is set (1) to enable 
 * Rx mode or reset to enable Tx mode.
**/
#define PWR_UP  1 // Bit 1
#define PRIM_RX 0 // Bit 0

/**
 * STATUS register interrupts:
 * 
 * RX_DR (bit 6) is the data ready Rx FIFO 
 * interrupt. Asserted (1) when new data 
 * arrives in Rx FIFO. Write 1 to clear.
 * 
 * TX_DS (bit 5) is the data sent Tx FIFO 
 * interrupt. Asserted on packet transmission
 * on Tx. If AUTO_ACK is activated, this bit 
 * is set high, only when ACK is received.
 * 
 * MAX_RT (bit 4) is the maximum number of 
 * Tx retransmits interrupt. Write 1 to clear 
 * bit. If MAX_RT is asserted, then it must 
 * be cleared to enable further communication. 
**/
#define RX_DR   6 // Bit 6
#define TX_DS   5 // Bit 5
#define MAX_RT  4 // Bit 4

/**
 * FIFO_STATUS register RX_EMPTY flag bit:
 * 
 * RX_EMPTY (bit 0) is set (1) when the 
 * RX FIFO is empty and reset (0) when
 * there is data in the RX FIFO.
**/
#define RX_EMPTY 0 // Bit 0

#endif