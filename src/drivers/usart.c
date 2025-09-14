#include "drivers/usart.h"
#include <stdint.h>


void usart_init(USART_Handler_t *USARTHandler) {
    uint16_t ubbrn = 0;

    // Calculate baud rate register value based on double speed setting
    if (!(USARTHandler->USARTConfig.doubleSpeed)) {
        // Asynchronous Normal mode (U2Xn = 0)
        ubbrn = ((F_CPU / (16 * USARTHandler->USARTConfig.baudRate)) - 1);
    } else {
        // Asynchronous Double Speed mode (U2Xn = 1)
        UCSR0A |= USART_U2X; // Enable double speed
        ubbrn = (F_CPU / (8 * USARTHandler->USARTConfig.baudRate)) - 1;
    }

    // Set baud rate registers (split 16-bit value into two 8-bit registers)
    UBRR0L = ubbrn;               // Set low byte
    UBRR0H = (ubbrn >> 8);        // Set high byte

    // Set frame format: data bits, stop bits, parity
    uint8_t ucsrc = 0;

    // Set number of data bits (5, 6, 7, 8, 9)
    switch (USARTHandler->USARTConfig.uartByteSize) {
        case 5: ucsrc |= 0; break; // 5-bit
        case 6: ucsrc |= (1 << 1); break; // 6-bit
        case 7: ucsrc |= (1 << 2); break; // 7-bit
        case 8: ucsrc |= (3 << 1);  break; // 8-bit
        case 9: ucsrc |= (1 << 1) | (1 << 2) | (1 << 6); break; // 9-bit
        default: ucsrc |= (1 << 1) | (1 << 2); break; // Default to 8-bit
    }

    // Set number of stop bits (1 or 2)
    if (USARTHandler->USARTConfig.stopBitQuantity == 2) {
        UCSR0C |= (1 << 3); // Set USBS for 2 stop bits
    }

    // Set parity (0: None, 1: Even, 2: Odd)
    switch (USARTHandler->USARTConfig.parityMode) {
        case 1: ucsrc |= (1 << 4); break; // Even
        case 2: ucsrc |= (1 << 4) | (1 << 5); break; // Odd
        default: break; // None
    }

    // Write frame format to UCSRC register
    UCSR0C = ucsrc;

    // Enable transmitter and receiver
    UCSR0B |= (1 << 3) | (1 << 4); // TXENn and RXENn
    // USART is now configured and ready for use
}


void usart_transmit(uint8_t *data, uint16_t length)
{
    // Transmit 'length' bytes from the data buffer
    // Wait for transmit buffer to be empty (UDREn bit in UCSRA)
    while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
    for (uint16_t i = 0; i < length; i++) {
        // Put data into buffer, sends the byte
        UDR0 = data[i];
        while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
    }
}

void usart_receive(uint8_t *data, uint16_t length) {
    // Receive 'length' bytes into the data buffer
    for (uint16_t i = 0; i < length; i++) {
        while (( UCSR0A & (1<<RXC0)) == 0) {}; // Do nothing until UDR is ready
        // Get and return received data from buffer
        data[i] = UDR0;
    }
}
