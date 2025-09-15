#include "aux/usart_esp01s.h"
#include "config.h"

#define ESP_RESP_SMALL_LEN      16
#define ESP_RESP_MEDIUM_LEN     64
#define ESP_RESP_BIG_LEN        254


// Helper function for safe USART send/receive with retries (void usart_receive version)
int8_t safe_usart_transceive(const char *tx, char *rx, size_t rx_size, const char *token, int max_retries, uint16_t delay_ms_tx, uint16_t delay_ms_rx) {
    int retries = 0;
    for (retries = 0; retries < max_retries; retries++) {
        if (delay_ms_tx) {
            for (uint16_t i = 0; i < delay_ms_tx; i++) {
                _delay_ms(1);
            }
        }
        usart_transmit((uint8_t*)tx, strlen(tx));
        // Optional delay for ESP01S response stabilization
        if (delay_ms_rx) {
            for (uint16_t i = 0; i < delay_ms_rx; i++) {
                _delay_ms(1);
            }
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
    char receiveBuffer[ESP_RESP_SMALL_LEN];

    // 1. Test communication
    if (safe_usart_transceive("AT\r\n", receiveBuffer, ESP_RESP_SMALL_LEN, "OK", 3, 1500, 0)) {
        return -1;
    }

    // 2. Set USART configuration: 115200 baud, 8N1, no flow control
    if (safe_usart_transceive("AT+UART_DEF=115200,8,1,0,0\r\n", receiveBuffer, ESP_RESP_SMALL_LEN, "OK", 3, 10, 0)) {
        return -2;
    }

    return 0; // Success
}

int8_t esp01s_setup(USART_Handler_t *pToUSARTx){
    char receiveBuffer[ESP_RESP_MEDIUM_LEN];
    char transmitterBuffer[ESP_RESP_MEDIUM_LEN];

    // 1. Test communication
    if (safe_usart_transceive("AT\r\n", receiveBuffer, ESP_RESP_SMALL_LEN, "OK", 3, 10, 0)) {
        return -1;
    }

    // 2. Set WiFi mode to station
    if (safe_usart_transceive("AT+CWMODE=1\r\n", receiveBuffer, ESP_RESP_SMALL_LEN, "OK", 3, 10, 0)) {
        return -2;
    }

    // 4. Set single connection mode (optional, but recommended)
    if (safe_usart_transceive("AT+CIPMUX=0\r\n", receiveBuffer, ESP_RESP_SMALL_LEN, "OK", 3, 10, 0)) {
        return -4;
    }

    // 5. Open TCP connection (AT+CIPSTART)
    snprintf(transmitterBuffer, sizeof(transmitterBuffer), "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", WEB_HOST);

    if (safe_usart_transceive(transmitterBuffer, receiveBuffer, ESP_RESP_MEDIUM_LEN, "OK", 3, 10, 0) &&
        safe_usart_transceive(transmitterBuffer, receiveBuffer, ESP_RESP_MEDIUM_LEN, "ALREADY CONNECTED", 1, 10, 0)) {
        return -5;
    }

    return 0; // Success
}

int8_t esp01s_send_temperature(USART_Handler_t *pToUSARTx, float temperature) {
    char httpRequest[ESP_RESP_BIG_LEN];
    char sendDataAT[ESP_RESP_BIG_LEN];
    char receiveBuffer[ESP_RESP_BIG_LEN];

    snprintf(httpRequest, sizeof(httpRequest),
        "GET /api.php?type=temperature&sensor=bme280&value=%.1f&unit=celsius&device=HOME_EXT1 HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", temperature, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    // Wait for prompt '>' (short response buffer is enough)
    if (safe_usart_transceive(sendDataAT, receiveBuffer, ESP_RESP_SMALL_LEN, ">", 3, 10, 0)) {
        return 1;
    }
    // Send request and wait for SEND OK in a medium sized buffer (server responses may be longer)
    if (safe_usart_transceive(httpRequest, receiveBuffer, ESP_RESP_MEDIUM_LEN, "SEND OK", 3, 10, 0)) {
        return 2;
    }
    return 0;
}

int8_t esp01s_send_pressure(USART_Handler_t *pToUSARTx, float pressure) {
    char httpRequest[ESP_RESP_BIG_LEN];
    char sendDataAT[ESP_RESP_BIG_LEN];
    char receiveBuffer[ESP_RESP_BIG_LEN];

    snprintf(httpRequest, sizeof(httpRequest),
        "GET /api.php?type=pressure&sensor=bme280&value=%.1f&unit=hpa&device=HOME_EXT1 HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", pressure, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    if (safe_usart_transceive(sendDataAT, receiveBuffer, ESP_RESP_SMALL_LEN, ">", 3, 10, 0)) {
        return -1;
    }
    if (safe_usart_transceive(httpRequest, receiveBuffer, ESP_RESP_MEDIUM_LEN, "SEND OK", 3, 10, 0)) {
        return -2;
    }
    return 0;
}

int8_t esp01s_send_humidity(USART_Handler_t *pToUSARTx, float humidity) {
    char httpRequest[ESP_RESP_BIG_LEN];
    char sendDataAT[ESP_RESP_BIG_LEN];
    char receiveBuffer[ESP_RESP_BIG_LEN];

    snprintf(httpRequest, sizeof(httpRequest),
        "GET /api.php?type=humidity&sensor=bme280&value=%.1f&unit=percent&device=HOME_EXT1 HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", humidity, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    if (safe_usart_transceive(sendDataAT, receiveBuffer, ESP_RESP_SMALL_LEN, ">", 3, 10, 0)) {
        return -1;
    }
    if (safe_usart_transceive(httpRequest, receiveBuffer, ESP_RESP_MEDIUM_LEN, "SEND OK", 3, 10, 0)) {
        return -2;
    }
    return 0;
}

int8_t esp01s_send_rain(USART_Handler_t *pToUSARTx, uint8_t isRaining) {
    char httpRequest[ESP_RESP_BIG_LEN];
    char sendDataAT[ESP_RESP_BIG_LEN];
    char receiveBuffer[ESP_RESP_BIG_LEN];

    snprintf(httpRequest, sizeof(httpRequest),
        "GET /api.php?type=rain&sensor=mh_rain&value=%u&unit=boolean&device=HOME_EXT1 HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", isRaining, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    if (safe_usart_transceive(sendDataAT, receiveBuffer, ESP_RESP_SMALL_LEN, ">", 3, 10, 0)) {
        return -1;
    }
    if (safe_usart_transceive(httpRequest, receiveBuffer, ESP_RESP_MEDIUM_LEN, "SEND OK", 3, 10, 0)) {
        return -2;
    }
    return 0;
}
