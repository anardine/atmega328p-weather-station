#ifndef GPIO_MHRAIN_H
#define GPIO_MHRAIN_H

#include "drivers/atmega328p.h"
#include "drivers/gpio.h"

uint8_t read_rain(GPIO_handler_t *pToGPIOCx);


#endif // GPIO_MHRAIN_H