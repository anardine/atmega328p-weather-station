#include "aux/gpio_mhrain.h"


uint8_t read_rain(GPIO_handler_t *pToGPIOCx) {

    uint8_t state = 0;
    state = (PINC >> pToGPIOCx->gpioConfig.pinNumber) & 0x01;
    // return the value
    return state;

}