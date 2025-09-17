#ifndef TWI_H
#define TWI_H

#include <stdint.h>
#include "drivers/atmega328p.h"
#include "drivers/gpio.h"

typedef struct {
    uint8_t speed;      // TWI clock speed (e.g., 100kHz, 400kHz)
    uint8_t ownAddress; // Device own address (if used in slave mode)
} TWI_config_t;

typedef struct {
    TWI_config_t TWIConfig;  // TWI configuration structure
} TWI_handler_t;


/**
 * @brief Initializes the TWI (Two-Wire Interface) peripheral with the specified configuration.
 *
 * This function configures the TWI hardware according to the settings
 * provided in the TWI_handler_t structure. It typically sets the clock speed,
 * own address (for slave mode), and enables or disables internal pull-ups.
 *
 * @param twiHandler Pointer to a TWI_handler_t structure containing configuration.
 */
void twi_init(TWI_handler_t *twiHandler);

/**
 * @brief Sends data over TWI to a specific address.
 *
 * This function transmits a buffer of data to a target device on the TWI bus.
 *
 * @param twiHandler Pointer to TWI handler.
 * @param address 7-bit TWI address of the target device.
 * @param data Pointer to data buffer to send.
 * @param length Number of bytes to send.
 * @return 0 on success, non-zero on error.
 */
int twi_write(uint8_t address, const uint8_t *data, uint16_t length);

/**
 * @brief Receives data over TWI from a specific address.
 *
 * This function reads a buffer of data from a target device on the TWI bus.
 *
 * @param twiHandler Pointer to TWI handler.
 * @param address 7-bit TWI address of the target device.
 * @param data Pointer to buffer to store received data.
 * @param length Number of bytes to read.
 * @return 0 on success, non-zero on error.
 */
int twi_read(uint8_t address, uint8_t *data, uint16_t length);


#endif // end I2C_H