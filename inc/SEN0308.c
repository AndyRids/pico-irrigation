#include "SEN0308.h"

float max_voltage = 2.93; // Max voltage during calibration (in air, 0%)
float min_voltage = 0.40; // Min voltage during calibration (in water, 100%)

/**
 * A primary transmitter (PTX) will interpret data 
 * from a DFRobot moisture sensor using ADC, and 
 * will Tx the data to the primary receiver (PRX).
 * 
 * 1. Initialise ADC hardware.
 * 
 * 2. Prepare PIN_ADC GPIO for use with ADC.
 * 
 * 3. Select the ADC input (0).
 * 
 * NOTE: RPi Pico ADC GPIO are 26 to 29 inclusive.
 * ADC input 0 - 3 are GPIO 26 - 29 respectively.
 */
void init_adc() {
  adc_init(); // Initialise ADC hardware

  // Make sure GPIO is high-impedance, no pullups etc
  adc_gpio_init(PIN_ADC);

  // Select ADC input 1 (GPIO27)
  adc_select_input(ADC_INPUT);

  // Initialise GPIO used to power DFRobot sensor
  gpio_init(PIN_DFR);

  // Set DFRobot sensor GPIO direction to out
  gpio_set_dir(PIN_DFR, GPIO_OUT);
}

void calibrate() {
  uint16_t adc_conversion;
  uint8_t percentage;
  float voltage;

  // Get an ADC reading
  adc_conversion = adc_read();

  // Calculate voltage based on 3.3 system voltage and Pico 12-bit ADC
  voltage = adc_conversion * (3.3f / (1 << 12));

  // Calculate the soil moisture as a percentage
  percentage = 100 - roundf((voltage - min_voltage) / (max_voltage - min_voltage) * 100);

  printf("Voltage: %f V, Percentage: %d\n", voltage, percentage);
}

uint8_t read_moisture() {
    uint16_t adc_conversion;
    uint8_t percentage, buffer[4];
    float voltage, sum;

    // Power DFRobot sensor
    gpio_put(PIN_DFR, HIGH);

    // Wait for ADC or you get an erroneous initial reading
    sleep_ms(1000);

    for (size_t i = 0; i < 5; i++)
    {
      // Get an ADC reading
      adc_conversion = adc_read();

      // Calculate voltage based on 3.3 system voltage and Pico 12-bit ADC
      voltage = adc_conversion * (3.3f / (1 << 12));

      // Calculate the soil moisture as a percentage
      percentage = 100 - roundf((voltage - min_voltage) / (max_voltage - min_voltage) * 100);

      // Store each reading to calculate average
      buffer[i] = percentage;

      printf("Voltage: %f V, Percentage: %d\n", voltage, percentage);

      sleep_ms(500);
    }

    // Turn off DFRobot sensor
    gpio_put(PIN_DFR, LOW);

    // Calculate sum of the 5 ADC readings
    for (size_t i = 0; i < 5; i++)
    {
      sum += buffer[i];
    }

    // Calculate average percentage
    percentage = roundf(sum / 5);

    // Return percentage value
    return percentage;
}

