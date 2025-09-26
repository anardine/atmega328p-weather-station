#include "aux/usart_esp01s.h"
#include "config.h"

#define ESP_RESP_SMALL_LEN      16
#define ESP_RESP_MEDIUM_LEN     64
#define ESP_RESP_BIG_LEN        254


int8_t esp01s_init(USART_Handler_t *pToUSARTx) {
    char transmitterBuffer[ESP_RESP_BIG_LEN];

    _delay_ms(5000);
    // 1. Test communication
    usart_transmit((uint8_t*)"AT\r\n", 4);
    _delay_ms(50);

    // 2. Set WiFi mode to station
    usart_transmit((uint8_t*)"AT+CWMODE=1\r\n", 13);
    _delay_ms(100);

    // 4. Set single connection mode (optional, but recommended)
    usart_transmit((uint8_t*)"AT+CIPMUX=0\r\n",13);

    // in case of power loss, WIFI generally takes 30s to boot up. This is to account for this situation. 1 minute before connecting
    _delay_ms(60000);

    //connect to WIFI
    sprintf(transmitterBuffer, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PWD);
    usart_transmit((uint8_t*)transmitterBuffer, strlen(transmitterBuffer));
    _delay_ms(10000);

    return 0; // Success
}

int8_t esp01s_send_temperature(USART_Handler_t *pToUSARTx, float temperature) {
    char httpRequest[ESP_RESP_BIG_LEN];
    char sendDataAT[ESP_RESP_BIG_LEN];
    char transmitterBuffer[ESP_RESP_BIG_LEN];


    // 5. Open TCP connection (AT+CIPSTART)
    sprintf(transmitterBuffer, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", WEB_HOST);
    usart_transmit((uint8_t*)transmitterBuffer, strlen(transmitterBuffer));
    _delay_ms(10000);

    snprintf(httpRequest, sizeof(httpRequest),
        "GET /api.php?type=temperature&sensor=bme280&value=%.1f&unit=celsius&device=%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", temperature, BOARD_NAME, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    usart_transmit((uint8_t*)sendDataAT, strlen(sendDataAT));
    _delay_ms(500);

    usart_transmit((uint8_t*)httpRequest, strlen(httpRequest));
    _delay_ms(5000);

    return 0;
}

int8_t esp01s_send_pressure(USART_Handler_t *pToUSARTx, float pressure) {
    char httpRequest[ESP_RESP_BIG_LEN];
    char sendDataAT[ESP_RESP_BIG_LEN];
    char transmitterBuffer[ESP_RESP_BIG_LEN];


    // 5. Open TCP connection (AT+CIPSTART)
    sprintf(transmitterBuffer, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", WEB_HOST);
    usart_transmit((uint8_t*)transmitterBuffer, strlen(transmitterBuffer));
    _delay_ms(10000);

    snprintf(httpRequest, sizeof(httpRequest),
        "GET /api.php?type=pressure&sensor=bme280&value=%.1f&unit=mBar&device=%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", pressure, BOARD_NAME, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    usart_transmit((uint8_t*)sendDataAT, strlen(sendDataAT));
    _delay_ms(500);

    usart_transmit((uint8_t*)httpRequest, strlen(httpRequest));
    _delay_ms(5000);

    return 0;
}

int8_t esp01s_send_humidity(USART_Handler_t *pToUSARTx, float humidity) {
    char httpRequest[ESP_RESP_BIG_LEN];
    char sendDataAT[ESP_RESP_BIG_LEN];
    char transmitterBuffer[ESP_RESP_BIG_LEN];


    // 5. Open TCP connection (AT+CIPSTART)
    sprintf(transmitterBuffer, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", WEB_HOST);
    usart_transmit((uint8_t*)transmitterBuffer, strlen(transmitterBuffer));
    _delay_ms(10000);

    snprintf(httpRequest, sizeof(httpRequest),
        "GET /api.php?type=humidity&sensor=bme280&value=%.1f&unit=percent&device=%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", humidity, BOARD_NAME, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    usart_transmit((uint8_t*)sendDataAT, strlen(sendDataAT));
    _delay_ms(500);

    usart_transmit((uint8_t*)httpRequest, strlen(httpRequest));
    _delay_ms(5000);

    return 0;
}

int8_t esp01s_send_rain(USART_Handler_t *pToUSARTx, uint8_t isRaining) {
    char httpRequest[ESP_RESP_BIG_LEN];
    char sendDataAT[ESP_RESP_BIG_LEN];
    char transmitterBuffer[ESP_RESP_BIG_LEN];


    // 5. Open TCP connection (AT+CIPSTART)
    sprintf(transmitterBuffer, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", WEB_HOST);
    usart_transmit((uint8_t*)transmitterBuffer, strlen(transmitterBuffer));
    _delay_ms(10000);

    snprintf(httpRequest, sizeof(httpRequest),

        "GET /api.php?type=rain&sensor=mh_rain&value=%d&unit=bool&device=HOME_EXT1 HTTP/1.1\r\n"

        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", isRaining,BOARD_NAME, WEB_HOST);

    snprintf(sendDataAT, sizeof(sendDataAT), "AT+CIPSEND=%d\r\n", (int)strlen(httpRequest));

    usart_transmit((uint8_t*)sendDataAT, strlen(sendDataAT));
    _delay_ms(500);

    usart_transmit((uint8_t*)httpRequest, strlen(httpRequest));
    _delay_ms(5000);

    return 0;
}
