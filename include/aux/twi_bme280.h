#ifndef TWI_BME280_H
#define TWI_BME280_H


#include "drivers/twi.h"
#include "bme280.h"
#include <stdint.h>
#include <util/delay.h>


void bme280_set_twi_handler(TWI_handler_t *handler);
uint8_t bme280_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);
uint8_t bme280_i2c_write(uint8_t dev_id, uint8_t reg_addr, const uint8_t *data, uint16_t len);
void bme280_delay_ms(uint32_t period);



#endif // TWI_BME280_H