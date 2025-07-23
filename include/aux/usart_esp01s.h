#ifndef USART_ESP01S_H
#define USART_ESPO1S_H


#include "drivers/atmega328p.h"
#include "drivers/usart.h"
#include <stdio.h>
#include <string.h>

int8_t esp01s_send_data(USART_Handler_t *pToUSARTx, uint8_t *temperatureBuffer, uint8_t *pressureBuffer, uint8_t *humidityBuffer);



#endif // USART_ESPO1S_H