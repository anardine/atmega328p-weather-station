#ifndef USART_ESP01S_H
#define USART_ESPO1S_H


#include "drivers/atmega328p.h"
#include "drivers/usart.h"
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int8_t safe_usart_transceive(const char *tx, char *rx, size_t rx_size, const char *token, int max_retries, uint16_t delay_ms_tx, uint16_t delay_ms_rx);
int8_t esp01s_init(USART_Handler_t *pToUSARTx);
int8_t esp01s_setup(USART_Handler_t *pToUSARTx);
int8_t esp01s_send_temperature(USART_Handler_t *pToUSARTx, float temperature);
int8_t esp01s_send_pressure(USART_Handler_t *pToUSARTx, float pressure);
int8_t esp01s_send_humidity(USART_Handler_t *pToUSARTx, float humidity);
int8_t esp01s_send_rain(USART_Handler_t *pToUSARTx, uint8_t isRaining);



#endif // USART_ESPO1S_H