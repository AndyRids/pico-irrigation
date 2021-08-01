# Raspberry Pi Pico Irrigation Project

A work in progress for an automated irrigation system. I wanted to learn C and the pico-sdk and how to interface with the NRF24L01 modules and so, I have coded this with the Pico from scratch in C (which I learnt for this project). I also wanted to make use of the IRQ pin on the NRF24.

The long-term plan is to have a Pico with an NRF24 acting as the primary receiver (PRX), which will operate a motorised ball valve, on instruction, from up to 6 Picos with an NRF24 and each operating a soil moisture sensor. I will be attempting to have the soil moisture sensor and NRF24L01 operating on seperate cores.

I've done some heavy commenting and hopefully I've done decent job with the code and people find it useful. This took a long time, due to faulty NRF24 modules and with a lack of information regarding this particular setup.

I'm using the Waveshare NRF24L01 modules as these are cheaper than the Sparkfun ones, but are guarenteed to work better than the cheaper, often faulty ones I've had on Ebay etc.
