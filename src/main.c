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
#include "drivers/bme280.h"
#include "aux/spi_w25q128fv.h"
#include "aux/usart_esp01s.h"
#include "aux/gpio_mhrain.h"
#include "aux/gpio_warning.h"
#include "config.h"

//definition of global variables
 volatile uint8_t global_timer_fetch = 0;
 //volatile uint32_t global_memory_page_tracker = 0;
 // struct bme280_dev bme280;
 // struct bme280_data sensor_data;
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

    // TODO change global fetch timer to 8 for aprox 1 minute refference before oploading final operational code
    if(global_timer_fetch == 1) { // around 60 seconds per action on this interrupt routine :276 on simavr for testing

        // char TimerComp[] = "Global Timer reached one minute. Saving Weather Data\n";
        // usart_transmit((uint8_t*)&TimerComp, strlen(TimerComp));

        volatile float temperature = 0;
        volatile float pressure = 0;
        volatile float humidity = 0;
        volatile uint8_t isRaining = 0;

        // Fetch the data from the BME280 sensor and save to variables
        //bme280_get_sensor_data(BME280_ALL, &sensor_data, &bme280);

        //buffer to get the data that was going to be sent to the ESP module
        char *errorBuffer;
        uint8_t errorBufferLength = 100;

        //temperature = sensor_data.temperature;
        // pressure = sensor_data.pressure;
        // humidity = sensor_data.humidity;
        isRaining = (!(read_rain(pToGPIOC2))); // negated because for MH sensor 0 is raining

        //using custom libs
        temperature = bme280_readTemperature(0); // in °C
        pressure = bme280_readPressure(0)/100.0; // in mbar
        humidity = bme280_readHumidity(0); // in %

        char tempData[60];
        char humData[60];
        char pressData[60];
        char rainData[60];

        // snprintf(tempData, sizeof(tempData),
        // "type=temperature&sensor=bme280&value=%.1f&unit=celsius\n", temperature);
        // usart_transmit((uint8_t*)&tempData, strlen(tempData));
        //
        // snprintf(pressData, sizeof(pressData),
        // "type=pressure&sensor=bme280&value=%df&unit=mbar\n", pressure);
        // usart_transmit((uint8_t*)&pressData, strlen(pressData));
        //
        // snprintf(humData, sizeof(humData),
        // "type=humidity&sensor=bme280&value=%df&unit=percent\n", humidity);
        // usart_transmit((uint8_t*)&humData, strlen(humData));
        //
        // snprintf(rainData, sizeof(rainData),
        // "type=rain&sensor=mh&value=%d&unit=bool\n\n", isRaining);
        // usart_transmit((uint8_t*)&rainData, strlen(rainData));



        //setup conn to send data - SENDING FIRST TO DEBUG CONSOLE
        //esp01s_setup(pToUSART0);

        // send the data to the server via ESP01-S
        // if(esp01s_send_temperature(pToUSART0, temperature,errorBuffer, errorBufferLength)) {
        //     // toggle warning to advise that posting was not complete
        //     toggle_warning(pToGPIOC1);
        //     //save errorBuffer to the flash memory
        //     //flash_write_data(pToSPI1, pGlobalMemTracker, errorBufferLength, errorBuffer);
        //
        //     //Increment the global tracket to keep rack to where to save on the memory
        //     //global_memory_page_tracker += FLASH_NEW_PAGE;
        // }
        esp01s_send_pressure(pToUSART0, pressure) ? toggle_warning(pToGPIOC1) : 0;
        esp01s_send_humidity(pToUSART0, humidity) ? toggle_warning(pToGPIOC1) : 0;
        esp01s_send_temperature(pToUSART0, temperature) ? toggle_warning(pToGPIOC1) : 0;
        esp01s_send_rain(pToUSART0, isRaining) ? toggle_warning(pToGPIOC1) : 0;


        // resets the counter
        global_timer_fetch = 0;
    }

    // adds to the global timer counter
    global_timer_fetch++;

    // resets the timer
    TCNT1 = 0x00;
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
    // char usartInit[] = "USART0 Initialized - 9600 8N1\n";
    // usart_transmit((uint8_t*)&usartInit, strlen(usartInit));
    //
    // char programInit[] = "Program Inititialization Started\n";
    // usart_transmit((uint8_t*)&programInit, strlen(programInit));

/* ----------------------- BEGINING OF PERIPHERALS INITIALIZATION ----------------------- */

/* ----------- TWI INITIALIZATION ----------- */
    // not needed. Initialized by the BME itslef
    //pToTWI1->TWIConfig.speed = 100;

/* ----------- END OF TWI INITIALIZATION ----------- */

/* ----------- BME280 INITIALIZATION ----------- */

    // init sensor
    i2c_init();
    bme280_init(0);

    // bme280.chip_id = BME280_I2C_ADDR_PRIM;
    // bme280.intf = BME280_I2C_INTF;
    // bme280.read = bme280_i2c_read;
    // bme280.write = bme280_i2c_write;
    // bme280.delay_us = bme280_delay_ms;
    //
    // bme280_init(&bme280);

/* ----------- END OF BME280 INITIALIZATION ----------- */

/* ----------- TIMER INITIALIZATION ----------- */

    pToTimer1->config.prescaler = 1024;
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
pToGPIOC2->pToGPIOx = GPIOC;
pToGPIOC2->gpioConfig.inputOrOutput = GPIO_INPUT;
pToGPIOC2->gpioConfig.pinNumber = 2;

gpio_init(pToGPIOC2);

/* ----------- END OF GPIO RAIN INITIALIZATION ----------- */

/* ----------- GPIO WARNING INITIALIZATION ----------- */
// pToGPIOC1->pToGPIOx = GPIOC;
// pToGPIOC1->gpioConfig.inputOrOutput = GPIO_OUTPUT;
// pToGPIOC1->gpioConfig.pinNumber = 1;
//
// gpio_init(pToGPIOC1);

/* ----------- END OF GPIO WARNING INITIALIZATION ----------- */

    // char programInitEnd[] = "Program Inititialization Completed\n";
    // usart_transmit((uint8_t*)&programInitEnd, strlen(programInitEnd));

/* ----------------------- MAIN LOOP ----------------------- */

while (1==1)
{


}
    
    return 0;
}