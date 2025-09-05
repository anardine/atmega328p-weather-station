#include "aux/gpio_warning.h"


void toggle_warning(GPIO_handler_t *pToGPIOCx) {
    //togle warning for one time
    PORTC ^= (ENABLE << pToGPIOCx->gpioConfig.pinNumber);
    for (int i = 0; i < 100000; i++);
    PORTC ^= (ENABLE << pToGPIOCx->gpioConfig.pinNumber);

}