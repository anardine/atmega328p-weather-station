// Initialize GPIO pin
#include "drivers/gpio.h"
#include <stdio.h>


void gpio_init(GPIO_handler_t *gpio) {

    // If the pin is configured as output
    if (gpio->gpioConfig.inputOrOutput == GPIO_OUTPUT) {
        // Set the corresponding bit in DDR register to configure as output
        gpio->pToGPIOx->ddr |= (ENABLE << gpio->gpioConfig.pinNumber);
    } 

    // If the pin is configured as input with pull-up enabled
    if ((gpio->gpioConfig.pullUpOrPullDown == GPIO_PULL_UP) && (gpio->gpioConfig.inputOrOutput == GPIO_INPUT)) {
        // Set the corresponding bit in PORT register to enable pull-up resistor
        gpio->pToGPIOx->port |= (ENABLE << gpio->gpioConfig.pinNumber);
    } 
}


void gpio_write(GPIO_handler_t *gpio, uint8_t state) {
    // Check if the pin is configured as output
    if (gpio->gpioConfig.inputOrOutput == GPIO_OUTPUT) {
        if (state) {
            // Set the pin high
            gpio->pToGPIOx->port |= (ENABLE << gpio->gpioConfig.pinNumber);
        } else {
            // Set the pin low
            gpio->pToGPIOx->port &= ~(ENABLE << gpio->gpioConfig.pinNumber);
        }
    }
}
    


uint8_t gpio_read(GPIO_handler_t *gpio) {
    // Check if the pin is configured as input
    if (gpio->gpioConfig.inputOrOutput == GPIO_INPUT) {
        // Read the pin value from the PIN register
        uint8_t value = (gpio->pToGPIOx->pin >> gpio->gpioConfig.pinNumber) & 0x01;
        return value;
    } else {
        printf("This port and pin is configured as an output.\n");
        // Optionally, still return the pin state:
        return (gpio->pToGPIOx->pin >> gpio->gpioConfig.pinNumber) & 0x01;
    }
}