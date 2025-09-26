// Initialize GPIO pin
#include "drivers/gpio.h"
#include <stdio.h>


void gpioC_init(GPIO_handler_t *gpioC) {

    // If the pin is configured as output
    if (gpioC->gpioConfig.inputOrOutput == GPIO_OUTPUT) {
        // Set the corresponding bit in DDR register to configure as output
        DDRC |= (ENABLE << gpioC->gpioConfig.pinNumber);
    } 

    // If the pin is configured as input with pull-up enabled
    if ((gpioC->gpioConfig.pullUpOrPullDown == GPIO_PULL_UP) && (gpioC->gpioConfig.inputOrOutput == GPIO_INPUT)) {
        // Set the corresponding bit in PORT register to enable pull-up resistor
        PORTC |= (ENABLE << gpioC->gpioConfig.pinNumber);
    } 
}


void gpioC_write(GPIO_handler_t *gpioC, uint8_t state) {
    // Check if the pin is configured as output
    if (gpioC->gpioConfig.inputOrOutput == GPIO_OUTPUT) {
        if (state) {
            // Set the pin high
            PORTC |= (ENABLE << gpioC->gpioConfig.pinNumber);
        } else {
            // Set the pin low
            PORTC &= ~(ENABLE << gpioC->gpioConfig.pinNumber);
        }
    }
}
    



uint8_t gpio_read(GPIO_handler_t *gpioc) {
        return ((PINC >> gpioc->gpioConfig.pinNumber) & 0x01);
}