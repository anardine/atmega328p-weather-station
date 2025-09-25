// Initialize GPIO pin
#include "drivers/gpio.h"
#include <stdio.h>


void gpio_init(GPIO_handler_t *gpioc) {

    // If the pin is configured as output
    if (gpioc->gpioConfig.inputOrOutput == GPIO_OUTPUT) {
        // Set the corresponding bit in DDR register to configure as output
        DDRC |= (ENABLE << gpioc->gpioConfig.pinNumber);
    } 

    // If the pin is configured as input with pull-up enabled
    if ((gpioc->gpioConfig.pullUpOrPullDown == GPIO_PULL_UP) && (gpioc->gpioConfig.inputOrOutput == GPIO_INPUT)) {
        // Set the corresponding bit in PORT register to enable pull-up resistor
        PORTC |= (ENABLE << gpioc->gpioConfig.pinNumber);
    } 
}


void gpio_write(GPIO_handler_t *gpioc, uint8_t state) {
    // Check if the pin is configured as output
    if (gpioc->gpioConfig.inputOrOutput == GPIO_OUTPUT) {
        if (state) {
            // Set the pin high
            PORTC |= (ENABLE << gpioc->gpioConfig.pinNumber);
        } else {
            // Set the pin low
            PORTC &= ~(ENABLE << gpioc->gpioConfig.pinNumber);
        }
    }
}
    


uint8_t gpio_read(GPIO_handler_t *gpioc) {
        return ((PINC >> gpioc->gpioConfig.pinNumber) & 0x01);
}