#ifndef UART_H
#define UART_H


#include <stdint.h>
#include "drivers/atmega328p.h"

typedef struct {
    uint32_t baudRate;         // Baud rate (e.g., 9600, 115200)
    uint8_t uartByteSize;      // Number of data bits (5, 6, 7, 8, 9)
    uint8_t stopBitQuantity;   // Number of stop bits (1 or 2)
    uint8_t parityMode;        // 0: None, 1: Even, 2: Odd
    uint8_t enableRx;          // 0: Disable, 1: Enable receiver
    uint8_t enableTx;          // 0: Disable, 1: Enable transmitter
    uint8_t doubleSpeed;       // 0: Normal, 1: Double speed (U2X)
} UART_config_t;

typedef struct
{
    USART_TypeDef *pUSARTx;
    UART_config_t USARTConfig;
} USART_Handler_t;


void usart_init(USART_Handler_t *USARTHandler);

void usart_transmit(USART_Handler_t *USARTHandler);

void usart_receive(USART_Handler_t *USARTHandler);

#endif