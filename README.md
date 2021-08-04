# Raspberry Pi Pico Irrigation Project

A work in progress for an automated irrigation system. I wanted to learn C and the pico-sdk and how to interface with the NRF24L01 modules and so, I have coded this with the Pico from scratch in C (which I learnt for this project). I also wanted to make use of the IRQ pin on the NRF24.

The long-term plan is to have a Pico with an NRF24 acting as the primary receiver (PRX), which will operate a motorized ball valve, on instruction, from up to 6 Picos with an NRF24 and each operating a soil moisture sensor. I will be attempting to have the soil moisture sensor and NRF24L01 operating on separate cores.

I'm using the Waveshare NRF24L01 modules as these are less expensive than the Sparkfun NRF24 modules, but are guaranteed to work better than the cheaper, often inferior ones on Ebay etc. I'm also using the Cytron Maker Pi Pico boards for prototyping and development. The GPIO LED indicators are especially useful for visually identifying issues with the IRQ pin code and with the CE pin, in RX MODE. I use one of the three programmable push buttons to Tx a message.