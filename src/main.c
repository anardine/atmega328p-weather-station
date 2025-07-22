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
#include "aux/spi_w25q128fv.h"

// Include driver for BME280
#include <bme280.h>

static volatile uint8_t global_timer_fetch = 0;
static volatile uint8_t global_memory_page_tracker = 0;
struct bme280_dev bme280;
struct bme280_data sensor_data;
TIMER_Handler_t *pToTimer1;
SPI_Handler_t *pToSPI1;
TWI_handler_t *pToTWI1;



/* ----------------------- BEGINING OF INTERRUPT ROUTINES ----------------------- */

// Timer interrupt routine
ISR (TIMER1_OVF_vect) {

    if(global_timer_fetch >= 8) { // around 60 seconds per action on this interrupt routine
        
        // Fetch the data from the BME280 sensor and save to variables
        bme280_get_sensor_data(BME280_ALL, &sensor_data, &bme280);

        static volatile double temperature = 0;
        static volatile double pressure = 0;
        static volatile double humidity = 0;

        //uint8 pointers to data
        uint8_t *pToTemp = &temperature;
        uint8_t *pToPress = &pressure;
        uint8_t *pToHumid = &humidity;

        //buffers to store data before saving to spi in uint8 format
        uint8_t temperatureBuffer[sizeof(double)];
        uint8_t pressureBuffer[sizeof(double)];
        uint8_t humidityBuffer[sizeof(double)];

        temperature = sensor_data.temperature;
        pressure = sensor_data.pressure;
        humidity = sensor_data.humidity;

        // sends this data to the flash memory for storage and further use.

        //flash_write_data(pToSPI,0x24,sizeof(double), tempBuffer);

        // resets the counter
        global_timer_fetch = 0;
    }

    // adds to the global timer counter
    global_timer_fetch++;

    // resets the timer 
    TIMER1->tcnt = 0x00;
}


/* ----------------------- END OF INTERRUPT ROUTINES ----------------------- */


int main(void) {

/* ----------------------- BEGINING OF PERIPHERALS INITIALIZATION ----------------------- */

/* ----------- TWI INITIALIZATION ----------- */

pToTWI1->pTWIx = TWI1;
pToTWI1->TWIConfig.speed = 100;
/* ----------- END OF TWI INITIALIZATION ----------- */

/* ----------- BME280 INITIALIZATION ----------- */
bme280_set_twi_handler(pToTWI1);

bme280.chip_id = BME280_I2C_ADDR_PRIM;
bme280.intf = BME280_I2C_INTF;
bme280.read = bme280_i2c_read;
bme280.write = bme280_i2c_write;
bme280.delay_us = bme280_delay_ms;

bme280_init(&bme280);

/* ----------- END OF BME280 INITIALIZATION ----------- */

/* ----------- TIMER INITIALIZATION ----------- */

pToTimer1->config.prescaler = 1024;
pToTimer1->pTIMERx = TIMER1;

timer1_init(pToTimer);

/* ----------- END OF TIMER INITIALIZATION ----------- */

/* ----------- FLASH INITIALIZATION ----------- */
pToSPI1->pToSPIx = SPI1;
pToSPI1->SPIConfig.dataOrder =SPI_DATA_ORDER_MSB;
pToSPI1->SPIConfig.mode = SPI_MODE_MASTER;
// check if more spi configs need to be set to commuicate well with the winbond flash



/* ----------------------- MAIN LOOP ----------------------- */

while (1)
{
    
}
    
    return 0;
}