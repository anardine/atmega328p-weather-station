// #include "aux/twi_bme280.h"
//
// // Wrapper for BME280 I2C read (matches Bosch BME280 API signature)
// BME280_INTF_RET_TYPE bme280_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
//     uint8_t dev_id = *((uint8_t *)intf_ptr); // BME280 expects intf_ptr to be pointer to device address
//     // Write register address
//     if (twi_write(dev_id, &reg_addr, 1) != 0) return -1;
//     // Read data
//     if (twi_read(dev_id, reg_data, (uint16_t)len) != 0) return -1;
//     return 0;
// }
//
// // Wrapper for BME280 I2C write
// BME280_INTF_RET_TYPE bme280_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
//     uint8_t buf[len + 1];
//     uint8_t dev_id = *((uint8_t *)intf_ptr);
//     buf[0] = reg_addr;
//     for (uint32_t i = 0; i < len; i++) buf[i + 1] = reg_data[i];
//     if (twi_write(dev_id, buf, len + 1) != 0) return -1;
//     return 0;
// }
//
// // // Wrapper for BME280 delay (matches Bosch BME280 API signature)
// void bme280_delay_ms(uint32_t period, void *intf_ptr) {
//     (void)intf_ptr; // Unused parameter
//     while (period--) _delay_ms(1);
// }
