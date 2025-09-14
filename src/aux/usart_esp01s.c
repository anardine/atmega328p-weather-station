#include "aux/usart_esp01s.h"
#include "config.h"

// Helper function for safe USART send/receive with retries (void usart_receive version)
static int safe_usart_transceive(const char *tx, char *rx, size_t rx_size, const char *token, int max_retries, uint16_t delay_ms) {
    int retries;
    for (retries = 0; retries < max_retries; retries++) {
        usart_transmit((uint8_t*)tx, strlen(tx));
        // Optional delay for ESP01S response stabilization
        if (delay_ms) {
            for (volatile uint32_t dly = 0; dly < ((uint32_t)delay_ms) * 800; dly++); // crude software delay ~1ms per iteration at 8Mhz
        }
        usart_receive((uint8_t*)rx, rx_size - 1);
        rx[rx_size-1] = 0; // Always null terminate
        if (strstr(rx, token) != NULL) {
            return 0; // success
        }
    }
    return -1;
}

int8_t esp01s_init(USART_Handler_t *pToUSARTx) {
    char receiveBuffer[100];

    _delay_ms(1500);    // delay for starting up
    // 1. Test communication
    if (safe_usart_transceive("AT\r\n", receiveBuffer, sizeof(receiveBuffer), "OK", 3, 5)) {
        return -1;
    }

    // 2. Set USART configuration: 115200 baud, 8N1, no flow control
    if (safe_usart_transceive("AT+UART_DEF=115200,8,1,0,0\r\n", receiveBuffer, sizeof(receiveBuffer), "OK", 3, 5)) {
        return -2;
    }

    return 0; // Success
}

int8_t esp01s_setup(USART_Handler_t *pToUSARTx){
    char receiveBuffer[100];
    char cmdBuffer[100];

    // 1. Test communication
    if (safe_usart_transceive("AT\r\n", receiveBuffer, sizeof(receiveBuffer), "OK", 3, 100)) {
        return -1;
    }

    // 2. Set WiFi mode to station
    if (safe_usart_transceive("AT+CWMODE=1\r\n", receiveBuffer, sizeof(receiveBuffer), "OK", 3, 100)) {
        return -2;
    }

    // 4. Set single connection mode (optional, but recommended)
    if (safe_usart_transceive("AT+CIPMUX=0\r\n", receiveBuffer, sizeof(receiveBuffer), "OK", 3, 100)) {
        return -4;
    }

    // 5. Open TCP connection (AT+CIPSTART)
    snprintf(cmdBuffer, sizeof(cmdBuffer), "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", WEB_HOST);
    // Accept "OK" or "ALREADY CONNECTED"
    if (safe_usart_transceive(cmdBuffer, receiveBuffer, sizeof(receiveBuffer), "OK", 3, 100) &&
        safe_usart_transceive(cmdBuffer, receiveBuffer, sizeof(receiveBuffer), "ALREADY CONNECTED", 1, 100)) {
        return -5;
    }

    return 0; // Success
}

int8_t esp01s_send_temperature(USART_Handler_t *pToUSARTx, float temperature) {
    char httpRequest[300];
    char sendDataAT[100];
    char receiveBuffer[100];

    snprintf(httpRequest, sizeof(httpRequest),
        "GET /api.php?type=temperature&sensor=bme280&value=%.1f&unit=celsius&device=HOME_EXT1 HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", temperature, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    // Wait for prompt '>'
    if (safe_usart_transceive(sendDataAT, receiveBuffer, sizeof(receiveBuffer), ">", 3, 100)) {
        return 1;
    }
    // Send request and wait for SEND OK
    if (safe_usart_transceive(httpRequest, receiveBuffer, sizeof(receiveBuffer), "SEND OK", 3, 100)) {
        return 2;
    }
    return 0;
}

int8_t esp01s_send_pressure(USART_Handler_t *pToUSARTx, float pressure) {
    char httpRequest[300];
    char sendDataAT[100];
    char receiveBuffer[100];

    snprintf(httpRequest, sizeof(httpRequest),
        "GET /api.php?type=pressure&sensor=bme280&value=%.2f&unit=hpa&device=HOME_EXT1 HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", pressure, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    if (safe_usart_transceive(sendDataAT, receiveBuffer, sizeof(receiveBuffer), ">", 3, 100)) {
        return -1;
    }
    if (safe_usart_transceive(httpRequest, receiveBuffer, sizeof(receiveBuffer), "SEND OK", 3, 100)) {
        return -2;
    }
    return 0;
}

int8_t esp01s_send_humidity(USART_Handler_t *pToUSARTx, float humidity) {
    char httpRequest[300];
    char sendDataAT[100];
    char receiveBuffer[100];

    snprintf(httpRequest, sizeof(httpRequest),
        "GET /api.php?type=humidity&sensor=bme280&value=%.2f&unit=percent&device=HOME_EXT1 HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", humidity, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    if (safe_usart_transceive(sendDataAT, receiveBuffer, sizeof(receiveBuffer), ">", 3, 100)) {
        return -1;
    }
    if (safe_usart_transceive(httpRequest, receiveBuffer, sizeof(receiveBuffer), "SEND OK", 3, 100)) {
        return -2;
    }
    return 0;
}

int8_t esp01s_send_rain(USART_Handler_t *pToUSARTx, uint8_t isRaining) {
    char httpRequest[300];
    char sendDataAT[100];
    char receiveBuffer[100];

    snprintf(httpRequest, sizeof(httpRequest),
        "GET /api.php?type=rain&sensor=mh_rain&value=%u&unit=boolean&device=HOME_EXT1 HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", isRaining, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    if (safe_usart_transceive(sendDataAT, receiveBuffer, sizeof(receiveBuffer), ">", 3, 100)) {
        return -1;
    }
    if (safe_usart_transceive(httpRequest, receiveBuffer, sizeof(receiveBuffer), "SEND OK", 3, 100)) {
        return -2;
    }
    return 0;
}
