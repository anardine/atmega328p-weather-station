/**
 * @file main.c
 * @brief Main entry point for the application.
 *
 * This file contains the main function and initializes the core components
 * required to run the application. It sets up the environment, handles
 * application-level configuration, and starts the main execution loop.
 *
 * @author Alessandro Nardinelli
 * @date July 14th 2025
 * @version 1.0
 */

// Includes all atmega328p drivers
#include "drivers/gpio.h"
#include "drivers/spi.h"
#include "drivers/timer.h"
#include "drivers/twi.h"
#include "drivers/usart.h"
#include "aux/twi_bme280.h"

// Include driver for BME280
#include <bme280.h>

int main() {

/* ----------------------- BEGINING OF PERIPHERALS CONFIGURATION ----------------------- */

/* ----------- GPIO TWI CONFIGURATION ----------- */
GPIO_handler_t *pToGPIOC;
pToGPIOC->pToGPIOx = GPIOC;
pToGPIOC->gpioConfig.pinNumber = 4;
pToGPIOC->gpioConfig.inputOrOutput = GPIO_OUTPUT;
pToGPIOC->gpioConfig.pullUpOrPullDown = GPIO_PULL_UP;

gpio_init(pToGPIOC);

pToGPIOC->gpioConfig.pinNumber = 5;

gpio_init(pToGPIOC);
/* ----------- END OF GPIO TWI CONFIGURATION ----------- */

/* ----------- TWI CONFIGURATION ----------- */
TWI_handler_t *pToTWI1;

pToTWI1->pTWIx = TWI1;
pToTWI1->TWIConfig.speed = 100;
/* ----------- END OF TWI CONFIGURATION ----------- */

/* ----------- BME280 CONFIGURATION ----------- */
bme280_set_twi_handler(pToTWI1);

struct bme280_dev bme280;
bme280.chip_id = BME280_I2C_ADDR_PRIM;
bme280.intf = BME280_I2C_INTF;
bme280.read = bme280_i2c_read;
bme280.write = bme280_i2c_write;
bme280.delay_us = bme280_delay_ms;

bme280_init(&bme280);

/* ----------- END OF BME280 CONFIGURATION ----------- */



    
    return 0;
}