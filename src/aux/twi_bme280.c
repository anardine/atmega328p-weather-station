#include "aux/twi_bme280.h"

static TWI_handler_t *bme280_twi_handler = NULL;

void bme280_set_twi_handler(TWI_handler_t *handler) {
    bme280_twi_handler = handler;
}

// Wrapper for BME280 I2C read
uint8_t bme280_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len) {
    if (!bme280_twi_handler) return -1;
    // Write register address, then read data
    if (twi_write(bme280_twi_handler, dev_id, &reg_addr, 1) != 0) return -1;
    if (twi_read(bme280_twi_handler, dev_id, data, len) != 0) return -1;
    return 0;
}

// Wrapper for BME280 I2C write
uint8_t bme280_i2c_write(uint8_t dev_id, uint8_t reg_addr, const uint8_t *data, uint16_t len) {
    if (!bme280_twi_handler) return -1;
    uint8_t buf[len + 1];
    buf[0] = reg_addr;
    for (uint16_t i = 0; i < len; i++) buf[i + 1] = data[i];
    if (twi_write(bme280_twi_handler, dev_id, buf, len + 1) != 0) return -1;
    return 0;
}

// Wrapper for BME280 delay
void bme280_delay_ms(uint32_t period) {
    while (period--) _delay_ms(1);
}
