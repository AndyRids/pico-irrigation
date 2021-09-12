# Raspberry Pi Pico Irrigation Project

A work in progress for an automated irrigation system. I wanted to learn C and the pico-sdk and how to interface with the NRF24L01 modules.

The long-term plan is to have a Pico with an NRF24 acting as the primary receiver (PRX), which will operate a motorized ball valve, on instruction, from up to 6 Picos with an NRF24 and each operating a soil moisture sensor.

**TODO**:

- [x] Have the rx_message function check the RX_FIFO register for other payloads after receipt of an initial payload
- [] Implement deep sleep on the PTX device after successful transmission of a payload and receipt of the auto-acknowledgement.
- [] Breakout the NRF24L01 driver into a separate repository
## Hardware used

* Raspberry Pi Pico
* Waveshare NRF24L01 RF Board (B)
* DFRobot Analog Waterproof Capacitive Soil Moisture Sensor
* Cytron Maker Pi Pico (prototyping & testing)
* U.S. Solid Motorized Ball Valve 1/2" (9-24V AC/DC & 2 Wire Auto Return)

## Current usage
### Files

* `NRF24L01.h` - Register addresses, instruction commands & bit mnemonics
* `NRF24.h` & `NRF24.c` - Functions to interact with the NRF24L01
* `SEN0308.h` & `SEN0308.c` - Functions to interact with the DFRobot moisture sensor 
* `GPIO_PINS.h` - Default GPIO pins in use

### Default configuration

Currently, the following configuration is used for the NRF24L01: 

* 1 Mbps data rate
* 2 byte CRC
* Auto-Acknowledgement
* Auto retransmit count of 10
* Auto retransmit delay of 750µS
* 5 byte address width
* RF output power of 0 dBm

#### SPI & ADC functions

SPI is initialized through `init_spi()` function. ADC is initialized through `init_adc()` function. Communication with the is NRF24L01
is over SPI and ADC is used to take a reading from the DFRobot soil moisture sensor.

```C
init_spi(); // Initialise SPI and GPIO pins
init_adc(); // Initialise ADC and GPIO pins
```
#### NRF24L01 functions

Configure NRF24 registers to act as a primary transmitter (PTX), passing the primary receiver's (PRX) data pipe
for communication with the PRX. `PRX_ADDR_P0` - `PRX_ADDR_P5` values are in `NRF24.c`.

```C
init_nrf24_ptx_registers(PRX_ADDR_P0);
```

Configure NRF24 registers for use as a PRX. Enables all data pipes and sets the address for each pipe. Function uses
the 5 byte address in `PRX_ADDR_P0` and `PRX_ADDR_P1` and one byte of the address in `PRX_ADDR_P2` - `PRX_ADDR_P5`.
NRF24 `RX_ADDR_P2` - `RX_ADDR_P5` registers use a unique 1 byte address and hold the same remaining 4 bytes as in the
`RX_ADDR_P1` register.

```C
init_nrf24_prx_registers();
```

Set a PTX to Tx mode or a PRX to Rx mode.

```C
set_mode(TX_MODE);
set_mode(RX_MODE);
```

`tx_message()` is used to transmit a payload to the PRX. A struct is used to send the PTX ID (0 - 5) and the moisture
percentage value.

```C
typedef enum { PTX_0, PTX_1, PTX_2, PTX_3, PTX_4, PTX_5 } ptx_id_t;

typedef struct
{ 
  ptx_id_t ptx_id : 8;
  uint8_t moisture : 8;
} payload_t;

void tx_message(payload_t* msg);

payload_t payload_tx = { PTX_0, 0 };

payload_tx.moisture = read_moisture();

tx_message(&payload_tx);
```

`rx_message()` is used by the PRX to receive a payload from a PTX. When an interrupt is asserted in the `STATUS` register,
indicating receipt of a payload - the function will read the payload over SPI, and store the ptx_id and moisture values in 
the payload_prx_t. The `rx_message` function will also determine the data pipe the payload was received on, through reading
bits 1 - 3 in the `STATUS` register. This is also stored in the payload_prx_t argument.

```C
typedef enum { PTX_0, PTX_1, PTX_2, PTX_3, PTX_4, PTX_5 } ptx_id_t;

typedef struct
{
  ptx_id_t ptx_id : 8;
  uint8_t data_pipe : 8;
  uint8_t moisture : 8;
} payload_prx_t;

void rx_message(payload_prx_t *msg);

payload_prx_t payload_rx;

rx_message(&payload_rx);
```

#### NRF24L01 IRQ handling

The IRQ pin is active-low and is driven low when one of the three interrupt bits in the `STATUS` register
is asserted. 

* TX_DR is asserted when packet received
* TX_DS is asserted when auto-acknowledge received
* MAX_RT is asserted when max retries reached

**NOTE**: If auto-acknowledge (AA) was not enabled, then the `TX_DS` would be asserted on every successful transmission of a payload. With AA enabled, it is only asserted when the auto-acknowledge payload is received from the PRX.

The pico-sdk `gpio_set_irq_enabled_with_callback` function (`#include "hardware/gpio.h"`) takes the IRQ GPIO pin, the event type that should cause an interrupt and the call back function which handles the interrupt (`gpio_irq_handler').

The interrupt handler adds a function pointer for the `check_irq_bit` function into the queue entry (queue_entry_t variable) and this queue entry is added to a call queue (queue_t variable). The main loop checks to see if the call queue is empty or not and runs the `check_irq_bit` function if an interrupt is asserted. The `check_irq_bit` return value corresponds to the interrupt type:

```C
typedef enum { NONE_ASSERTED, RX_DR_ASSERTED, TX_DS_ASSERTED, MAX_RT_ASSERTED } asserted_bit_t;
```
The main loop will then act accordingly. A PRX will call the `rx_message(&payload_rx)` function to receive the payload and check the transmitted moisture value. If the moisture is less than a certain percentage, then the GPIO pin switching on the motorized ball valve (via a transistor) will be driven high for 10 seconds.

```C
#include "pico/util/queue.h"

typedef struct { void *func; } queue_entry_t;

static queue_t call_queue;

uint8_t check_irq_bit(void); // NRF24.h

void gpio_irq_handler() {
  queue_entry_t entry = {&check_irq_bit};

  queue_add_blocking(&call_queue, &entry);
}

queue_init(&call_queue, sizeof(queue_entry_t), 6);

gpio_set_irq_enabled_with_callback(PIN_IRQ, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
```