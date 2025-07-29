#include "aux/gpio_mhrain.h"


uint8_t read_rain(GPIO_handler_t *pToGPIOx) {

    uint8_t state = 0;

    
    state = (pToGPIOx->pToGPIOx->pin >> pToGPIOx->gpioConfig.pinNumber) & 0x01;

    // return the value
    return state;

}