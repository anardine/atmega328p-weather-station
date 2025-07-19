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

// Include driver for BME280
#include <bme280.h>

int main() {



// Pointer to TWI handler (should be set in main)
static TWI_handler_t *bme280_twi_handler;


    
    return 0;
}