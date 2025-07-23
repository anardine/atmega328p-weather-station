#include "aux/usart_esp01s.h"

int8_t esp01s_send_temperature(USART_Handler_t *pToUSARTx, double temperature, uint64_t *timeCollected) {

    char transmitbuffer[50];
    char startConnAT[100] = "AT+CIPSTART=\"TCP\",\"server.com\",port\r\n"; // define properties
    char sendDataAT[100] = "AT+CIPSEND=length\r\n"; // define properties


    snprintf(transmitbuffer, sizeof(transmitbuffer), "temperature,bme280,%.2f,celcius,%u", temperature, timeCollected);

    usart_transmit(pToUSARTx, (uint8_t*)startConnAT, strlen(startConnAT));
    // now should check for ok command
    usart_transmit(pToUSARTx, (uint8_t*)sendDataAT, strlen(sendDataAT));
    // now should check for ok command
    usart_transmit(pToUSARTx, (uint8_t*)transmitbuffer, strlen(transmitbuffer));
    // now should check for ok command


}