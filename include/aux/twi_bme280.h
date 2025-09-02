#ifndef TWI_BME280_H
#define TWI_BME280_H


#include "drivers/twi.h"
#include "bme280.h"
#include <stdint.h>

TWI_handler_t *bme280_twi_handler = NULL;

void bme280_set_twi_handler(TWI_handler_t *handler);
BME280_INTF_RET_TYPE bme280_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
BME280_INTF_RET_TYPE bme280_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
// void bme280_delay_ms(uint32_t period, void *intf_ptr);

#endif // TWI_BME280_H