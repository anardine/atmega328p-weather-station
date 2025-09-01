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
#include "aux/usart_esp01s.h"
#include "aux/gpio_mhrain.h"
#include "aux/gpio_warning.h"
#include "config.h"

// Include driver for BME280
#include <bme280.h>

//definition of global variables
 volatile uint8_t global_timer_fetch = 0;
 volatile uint32_t global_memory_page_tracker = 0;
 struct bme280_dev bme280;
 struct bme280_data sensor_data;
 TIMER_Handler_t *pToTimer1;
 SPI_Handler_t *pToSPI1;
 TWI_handler_t *pToTWI1;
USART_Handler_t *pToUSART0;
GPIO_handler_t *pToGPIOC2;
GPIO_handler_t *pToGPIOC1;
uint8_t *pGlobalMemTracker;

/* ----------------------- BEGINING OF INTERRUPT ROUTINES ----------------------- */

//Timer interrupt routine
ISR(TIMER1_OVF_vect) {

    char InterruptInit[] = "ISR Routine Triggered\n";
    usart_transmit((uint8_t*)&InterruptInit, sizeof(InterruptInit));

    if(global_timer_fetch >= 8) { // around 60 seconds per action on this interrupt routine

        char TimerComp[] = "Global Timer reached 8... Saving Weather Data\n";
        usart_transmit((uint8_t*)&TimerComp, sizeof(TimerComp));

        // Fetch the data from the BME280 sensor and save to variables
        bme280_get_sensor_data(BME280_ALL, &sensor_data, &bme280);

        static volatile double temperature = 0;
        static volatile double pressure = 0;
        static volatile double humidity = 0;
        static volatile uint8_t isRaining = 0;

        //buffer to get the data that was going to be sent to the ESP module
        char *errorBuffer;
        uint8_t errorBufferLength = 100;

        temperature = sensor_data.temperature;
        pressure = sensor_data.pressure;
        humidity = sensor_data.humidity;
        isRaining = read_rain(pToGPIOC2);

        //setup conn to send data - SENDING FIRST TO DEBUG CONSOLE
        //esp01s_setup(pToUSART0);

        // send the data to the server via ESP01-S
        if(esp01s_send_temperature(pToUSART0, temperature,errorBuffer, errorBufferLength)) {
            // toggle warning to advise that posting was not complete
            toggle_warning(pToGPIOC1);
            //save errorBuffer to the flash memory
            //flash_write_data(pToSPI1, pGlobalMemTracker, errorBufferLength, errorBuffer);

            //Increment the global tracket to keep rack to where to save on the memory
            //global_memory_page_tracker += FLASH_NEW_PAGE;
        }
        // esp01s_send_pressure(pToUSART0, pressure) ? toggle_warning(pToGPIOC1) : 0;
        // esp01s_send_humidity(pToUSART0, humidity) ? toggle_warning(pToGPIOC1) : 0;
        // esp01s_send_rain(pToUSART0, isRaining) ? toggle_warning(pToGPIOC1) : 0;

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

    /* ----------- USART INITIALIZATION ----------- */
    pToUSART0->pUSARTx = USART0;
    pToUSART0->USARTConfig.baudRate = 9600;
    pToUSART0->USARTConfig.uartByteSize = 8;
    pToUSART0->USARTConfig.stopBitQuantity = 1;
    pToUSART0->USARTConfig.doubleSpeed = 0;
    pToUSART0->USARTConfig.parityMode = 0;

    // enabling USART for debugging through FTDI
    usart_init(pToUSART0);

    /* ----------- END OF USART INITIALIZATION ----------- */
    char usartInit[] = "USART0 Initialized - 9600 8N1\n";
    usart_transmit((uint8_t*)&usartInit, sizeof(usartInit));

    char programInit[] = "Program Inititialization Started\n";
    usart_transmit((uint8_t*)&programInit, sizeof(programInit));

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

    timer1_init(pToTimer1);

/* ----------- END OF TIMER INITIALIZATION ----------- */

/* ----------- FLASH INITIALIZATION ----------- */
// pToSPI1->pToSPIx = SPI1;
// pToSPI1->SPIConfig.dataOrder =SPI_DATA_ORDER_MSB;
// pToSPI1->SPIConfig.mode = SPI_MODE_MASTER;
//check if more spi configs need to be set to commuicate well with the winbond flash

//pGlobalMemTracker = (uint8_t *) &global_memory_page_tracker;

/* ----------- END OF FLASH INITIALIZATION ----------- */

/* ----------- ESP INITIALIZATION ----------- */

//esp01s_init(pToUSART0);

/* ----------- END OF ESP INITIALIZATION ----------- */

/* ----------- GPIO RAIN INITIALIZATION ----------- */
// pToGPIOC2->pToGPIOx = GPIOC;
// pToGPIOC2->gpioConfig.inputOrOutput = GPIO_INPUT;
// pToGPIOC2->gpioConfig.pinNumber = 2;
//
// gpio_init(pToGPIOC2);

/* ----------- END OF GPIO RAIN INITIALIZATION ----------- */

/* ----------- GPIO WARNING INITIALIZATION ----------- */
// pToGPIOC1->pToGPIOx = GPIOC;
// pToGPIOC1->gpioConfig.inputOrOutput = GPIO_OUTPUT;
// pToGPIOC1->gpioConfig.pinNumber = 1;
//
// gpio_init(pToGPIOC1);

/* ----------- END OF GPIO WARNING INITIALIZATION ----------- */

    sei(); //enable interrupting

    char programInitEnd[] = "Program Inititialization Completed\n";
    usart_transmit((uint8_t*)&programInitEnd, sizeof(programInitEnd));

/* ----------------------- MAIN LOOP ----------------------- */

while (1==1)
{


}
    
    return 0;
}