# Raspberry Pi Pico Irrigation Project

A work in progress for an automated irrigation system. I wanted to learn C and the pico-sdk and how to interface with the NRF24L01 modules and so, I have coded this with the Pico from scratch in C (which I learnt for this project). I also wanted to make use of the IRQ pin on the NRF24.

The long-term plan is to have a Pico with an NRF24 acting as the primary receiver (PRX), which will operate a motorized ball valve, on instruction, from up to 6 Picos with an NRF24 and each operating a soil moisture sensor. I will be attempting to have the soil moisture sensor and NRF24L01 operating on separate cores.

Hardware used; RPi Pico x 2, Waveshare NRF24L01 RF Board (B) x 2, DFRobot Analog Waterproof Capacitive Soil Moisture Sensor x 1, Cytron Maker Pi Pico (prototyping & testing) x 2, U.S. Solid Motorized Ball Valve 1/2" (9-24V AC/DC and 2 Wire Auto Return) x 1

Hardware note: Could use a cheaper capacitive moisture sensor and a solenoid valve in place of the motorized ball valve.

Update 16 Aug: Added code for reading soil moisture from DFRobot sensor and added code for the PRX to switch on the motorized ball valve if moisture is less than 60%. valve is activated using the second RPi pico core and a 10 second alarm.

TODO: More testing regarding the use of core1 and alarm feature. Need to solder a prototype to test PRX and motor activation. Looking to include circuit diagram for PRX board and PTX board.