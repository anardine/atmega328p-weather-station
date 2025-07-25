#ifndef USART_ESP01S_H
#define USART_ESPO1S_H


#include "drivers/atmega328p.h"
#include "drivers/usart.h"
#include <stdio.h>
#include <string.h>

void delay(void);
int8_t esp01s_setup(USART_Handler_t *pToUSARTx, char *ssid, char *pwd);
int8_t esp01s_send_temperature(USART_Handler_t *pToUSARTx, double temperature);
int8_t esp01s_send_pressure(USART_Handler_t *pToUSARTx, double pressure);
int8_t esp01s_send_humidity(USART_Handler_t *pToUSARTx, double humidity);


#endif // USART_ESPO1S_H