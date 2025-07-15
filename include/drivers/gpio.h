#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "drivers/atmega328p.h"

typedef struct
{
    volatile uint8_t    inputOrOutput;    
    volatile uint8_t    pinNumber;   
    volatile uint8_t    pullUpOrPullDown; 
}GPIO_config_t;

// GPIO Handler structure
typedef struct {
    GPIO_TypeDef *pToGPIOx;
    GPIO_config_t gpioConfig;
} GPIO_handler_t;

/**
 * @brief Initializes the specified GPIO pin.
 *
 * This function configures the GPIO pin according to the settings
 * provided in the GPIO_handler_t structure. It typically sets the pin
 * direction (input/output), pull-up/pull-down resistors, and other
 * relevant hardware configurations required for proper operation.
 *
 * @param gpio Pointer to a GPIO_handler_t structure that contains
 *             the configuration information for the specified GPIO pin.
 */
void gpio_init(GPIO_handler_t *gpio);

/**
 * @brief Writes a logic state to the specified GPIO pin.
 *
 * This function sets the output state of the GPIO pin to either high (1)
 * or low (0), depending on the value of the 'state' parameter. The pin
 * must be configured as an output before calling this function.
 *
 * @param gpio  Pointer to a GPIO_handler_t structure that identifies
 *              the GPIO pin to write to.
 * @param state Logic state to write to the pin (0 for low, 1 for high).
 */
void gpio_write(GPIO_handler_t *gpio, uint8_t state);

/**
 * @brief Reads the current logic state from the specified GPIO pin.
 *
 * This function returns the current input value of the GPIO pin. The pin
 * must be configured as an input before calling this function.
 *
 * @param gpio Pointer to a GPIO_handler_t structure that identifies
 *             the GPIO pin to read from.
 * @return uint8_t The current logic state of the pin (0 for low, 1 for high).
 */
uint8_t gpio_read(GPIO_handler_t *gpio);

#endif // GPIO_H