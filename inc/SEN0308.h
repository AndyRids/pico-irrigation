#ifndef SEN0308
#define SEN0308

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "GPIO_PINS.h"

extern float max_voltage;
extern float min_voltage;

/**
 * DFRobot waterproof soil moisture sensor (SKU: SEN0308)
 * 
 * Operating Voltage: 3.3 ~ 5.5 VDC
 * Output Voltage: 0 ~ 2.9VDC
 * 
 * Header file is used to make interfacing the moisture sensor
 * with the Raspberry Pi Pico easier. The RPi Pico uses 12-bit
 * ADC.
 */

// Initialise ADC and GPIO pins
void init_adc();

void calibrate();

uint8_t read_moisture();

#endif