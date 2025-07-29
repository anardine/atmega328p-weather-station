#ifndef USART_ESP01S_H
#define USART_ESPO1S_H


#include "drivers/atmega328p.h"
#include "drivers/usart.h"
#include <stdio.h>
#include <string.h>

void delay(void);
int8_t esp01s_init(USART_Handler_t *pToUSARTx);
int8_t esp01s_setup(USART_Handler_t *pToUSARTx);
int8_t esp01s_send_temperature(USART_Handler_t *pToUSARTx, double temperature);
int8_t esp01s_send_pressure(USART_Handler_t *pToUSARTx, double pressure);
int8_t esp01s_send_humidity(USART_Handler_t *pToUSARTx, double humidity);
int8_t esp01s_send_rain(USART_Handler_t *pToUSARTx, uint8_t isRaining);



#endif // USART_ESPO1S_H