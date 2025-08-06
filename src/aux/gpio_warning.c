#include "aux/gpio_warning.h"


void toggle_warning(GPIO_handler_t *pToGPIOx) {

    //togle warning for one time
    pToGPIOx->pToGPIOx->port ^= (ENABLE << pToGPIOx->gpioConfig.pinNumber);
    for (int i = 0; i < 100000; i++);
    pToGPIOx->pToGPIOx->port ^= (ENABLE << pToGPIOx->gpioConfig.pinNumber);


}