#include "aux/usart_esp01s.h"
#include "config.h"

void delay(void) {
    for (uint32_t i = 0; i < 10000; i++);
}

int8_t esp01s_init(USART_Handler_t *pToUSARTx) {
    char receiveBuffer[100];
    int retries;

    // 1. Test communication
    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)"AT\r\n", 4);
        delay();
        // usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        // if (strstr(receiveBuffer, "OK") != NULL)
        break;
    }
    if (retries == 3) return -1;

    // 2. Set USART configuration: 115200 baud, 8N1, no flow control
    // AT+UART_DEF=115200,8,1,0,0
    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)"AT+UART_DEF=115200,8,1,0,0\r\n", 28);
        delay();
        // usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        // if (strstr(receiveBuffer, "OK") != NULL)
            break;
    }
    if (retries == 3) return -2;

    return 0; // Success
}

int8_t esp01s_setup(USART_Handler_t *pToUSARTx){
    
    char receiveBuffer[100];
    char cmdBuffer[100];

    // Defensive logic: retry up to 3 times for each step
    int retries;

    // 1. Test communication
    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)"AT\r\n", 4);
        delay();
        // usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        // if (strstr(receiveBuffer, "OK") != NULL)
            break;
    }
    if (retries == 3) return -1;

    // 2. Set WiFi mode to station
    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)"AT+CWMODE=1\r\n", 14);
        delay();
        // usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        // if (strstr(receiveBuffer, "OK") != NULL)
            break;
    }
    if (retries == 3) return -2;

    // 3. Connect to WiFi using config.h definitions
    for (retries = 0; retries < 3; retries++) {
        snprintf(cmdBuffer, sizeof(cmdBuffer), "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PWD);
        usart_transmit((uint8_t*)cmdBuffer, strlen(cmdBuffer));
        delay();
        // usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        // if (strstr(receiveBuffer, "OK") != NULL || strstr(receiveBuffer, "WIFI CONNECTED") != NULL || strstr(receiveBuffer, "ALREADY CONNECTED") != NULL) break;
        // if (strstr(receiveBuffer, "ALREADY CONNECTED") != NULL) {
        //     // Already connected, proceed
        //     break;
        // }
        break; //remove when ready
    }
    if (retries == 3 && strstr(receiveBuffer, "ALREADY CONNECTED") == NULL) return -3;

    // 4. Set single connection mode (optional, but recommended)
    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)"AT+CIPMUX=0\r\n", 13);
        delay();
        // usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        // if (strstr(receiveBuffer, "OK") != NULL)
            break;
    }
    if (retries == 3) return -4;

    // 5. Open TCP connection (AT+CIPSTART)
    snprintf(cmdBuffer, sizeof(cmdBuffer), "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", WEB_HOST);
    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)cmdBuffer, strlen(cmdBuffer));
        delay();
        // usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        // if (strstr(receiveBuffer, "OK") != NULL || strstr(receiveBuffer, "ALREADY CONNECTED") != NULL)
            break;
    }
    if (retries == 3) return -5;

    return 0; // Success
}

int8_t esp01s_send_temperature(USART_Handler_t *pToUSARTx, double temperature, char *errorBuffer, uint16_t errorBufferSize) {
    static char dataToBeTransmitted[100];
    char httpRequest[300];
    char sendDataAT[100];
    char receiveBuffer[100];

    // Format data as HTTP POST form data
    snprintf(dataToBeTransmitted, sizeof(dataToBeTransmitted), 
        "type=temperature&sensor=bme280&value=%.2f&unit=celsius", temperature);
    
    // Create HTTP POST request
    snprintf(httpRequest, sizeof(httpRequest), 
        "POST /api.php HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s", 
        WEB_HOST, strlen(dataToBeTransmitted), dataToBeTransmitted);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", strlen(httpRequest));

    int retries;
    // Inform the device the size of data being transmitted
    for (retries = 0; retries <= 3; retries++) {
        usart_transmit((uint8_t*)sendDataAT, strlen(sendDataAT));
        delay();
        // usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        // if(strstr(receiveBuffer, ">") != NULL)
            break;
    }
    if (retries == 3) {
        strncpy(errorBuffer,dataToBeTransmitted,errorBufferSize);
        return 1;
    }

    for (retries = 0; retries <= 3; retries++) {
        usart_transmit((uint8_t*)httpRequest, strlen(httpRequest));
        delay();
        // usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        // if (strstr(receiveBuffer, "SEND OK") != NULL) return 0;
    }
    return 0; // remove when done
     if (retries == 3) {
        strncpy(errorBuffer,dataToBeTransmitted,errorBufferSize);
        return 2;
    }
}

int8_t esp01s_send_pressure(USART_Handler_t *pToUSARTx, double pressure) {
    char dataToBeTransmitted[150];
    char httpRequest[300];
    char sendDataAT[100];
    char receiveBuffer[100];

    // Format data as HTTP POST form data
    snprintf(dataToBeTransmitted, sizeof(dataToBeTransmitted), 
        "type=pressure&sensor=bme280&value=%.2f&unit=hpa", pressure);
    
    // Create HTTP POST request
    snprintf(httpRequest, sizeof(httpRequest), 
        "POST /api.php HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s", 
        WEB_HOST, strlen(dataToBeTransmitted), dataToBeTransmitted);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", strlen(httpRequest));

    int retries;
    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)sendDataAT, strlen(sendDataAT));
        delay();
        usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        if(strstr(receiveBuffer, ">") != NULL) break;
    }
    if (retries == 3) return -1;

    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)httpRequest, strlen(httpRequest));
        delay();
        usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        if (strstr(receiveBuffer, "SEND OK") != NULL) return 0;
    }
    return -2;
}

int8_t esp01s_send_humidity(USART_Handler_t *pToUSARTx, double humidity) {
    char dataToBeTransmitted[150];
    char httpRequest[300];
    char sendDataAT[100];
    char receiveBuffer[100];

    // Format data as HTTP POST form data (fix: was "pressure" should be "humidity")
    snprintf(dataToBeTransmitted, sizeof(dataToBeTransmitted), 
        "type=humidity&sensor=bme280&value=%.2f&unit=percent", humidity);
    
    // Create HTTP POST request
    snprintf(httpRequest, sizeof(httpRequest), 
        "POST /api.php HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s", 
        WEB_HOST, strlen(dataToBeTransmitted), dataToBeTransmitted);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", strlen(httpRequest));

    int retries;
    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)sendDataAT, strlen(sendDataAT));
        delay();
        usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        if(strstr(receiveBuffer, ">") != NULL) break;
    }
    if (retries == 3) return -1;

    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)httpRequest, strlen(httpRequest));
        delay();
        usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        if (strstr(receiveBuffer, "SEND OK") != NULL) return 0;
    }
    return -2;
}

int8_t esp01s_send_rain(USART_Handler_t *pToUSARTx, uint8_t isRaining) {
    char dataToBeTransmitted[150];
    char httpRequest[300];
    char sendDataAT[100];
    char receiveBuffer[100];

    // Format data as HTTP POST form data
    snprintf(dataToBeTransmitted, sizeof(dataToBeTransmitted), 
        "type=rain&sensor=mh_rain&value=%u&unit=boolean", isRaining);
    
    // Create HTTP POST request
    snprintf(httpRequest, sizeof(httpRequest), 
        "POST /api.php HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s", 
        WEB_HOST, strlen(dataToBeTransmitted), dataToBeTransmitted);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", strlen(httpRequest));

    int retries;
    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)sendDataAT, strlen(sendDataAT));
        delay();
        usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        if(strstr(receiveBuffer, ">") != NULL) break;
    }
    if (retries == 3) return -1;

    for (retries = 0; retries < 3; retries++) {
        usart_transmit((uint8_t*)httpRequest, strlen(httpRequest));
        delay();
        usart_receive((uint8_t*)receiveBuffer, sizeof(receiveBuffer));
        if (strstr(receiveBuffer, "SEND OK") != NULL) return 0;
    }
    return -2;
}
