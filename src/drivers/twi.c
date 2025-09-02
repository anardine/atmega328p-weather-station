
#include "drivers/twi.h"

void twi_init(TWI_handler_t *twiHandler) {
    // Set SCL frequency (default to 100kHz if not specified)
    uint32_t scl_freq = (twiHandler->TWIConfig.speed == 0) ? 100 : twiHandler->TWIConfig.speed;
    // Calculate TWBR value for desired SCL frequency
    uint32_t twbr_val = ((F_CPU / (scl_freq * 1000UL)) - 16) / 2;
    TWBR = (uint8_t)twbr_val; // Set bit rate register
    TWSR &= ~((1 << 0) | (1 << 1)); // Clear prescaler bits (TWPS0, TWPS1)

    // Set own address if in slave mode
    if (twiHandler->TWIConfig.ownAddress) {
        TWAR = (twiHandler->TWIConfig.ownAddress << 1); // Set slave address
    }
    // Enable the PULLUP on SDA and SCL pins
    GPIOC->port |= (1 << 4) | (1 << 5); // SDA = PC4, SCL = PC5

    // Enable TWI peripheral
    TWCR = TWI_ENABLE; //When TWI is enabled, the hardware automatically takes control of PC4/PC5 and makes them behave as open-drain
}


int twi_write(uint8_t address, const uint8_t *data, uint16_t length) {
    // Send START condition
    TWCR = TWI_START | TWI_ENABLE | TWI_INTERRUPT; // Request START
    while (!(TWCR & TWI_INTERRUPT)); // Wait for START to complete
    uint8_t status = TWSR & TW_STATUS_MASK; // Read status
    if (status != TW_START_VAL) return -1; // Check for successful START

    // Send SLA+W (address + write bit)
    TWDR = (address << 1); // Set address and write bit
    TWCR = TWI_ENABLE | TWI_INTERRUPT; // Start transmission
    while (!(TWCR & TWI_INTERRUPT)); // Wait for completion
    status = TWSR & TW_STATUS_MASK; // Read status
    if (status != TW_MT_SLA_ACK) return -2; // Check for ACK

    // Send data bytes
    for (uint16_t i = 0; i < length; i++) {
        TWDR = data[i]; // Load data byte
        TWCR = TWI_ENABLE | TWI_INTERRUPT; // Start transmission
        while (!(TWCR & TWI_INTERRUPT)); // Wait for completion
        status = TWSR & TW_STATUS_MASK; // Read status
        if (status != TW_MT_DATA_ACK) return -3; // Check for ACK
    }

    // Send STOP condition
    TWCR = TWI_ENABLE | TWI_STOP | TWI_INTERRUPT; // Request STOP
    return 0;
}


int twi_read(uint8_t address, uint8_t *data, uint16_t length) {
    // Send START condition
    TWCR = TWI_START | TWI_ENABLE | TWI_INTERRUPT; // Request START
    while (!(TWCR & TWI_INTERRUPT)); // Wait for START to complete
    uint8_t status = TWSR & TW_STATUS_MASK; // Read status
    if (status != TW_START_VAL) return -1; // Check for successful START

    // Send SLA+R (address + read bit)
    TWDR = (address << 1) | 1; // Set address and read bit
    TWCR = TWI_ENABLE | TWI_INTERRUPT; // Start transmission
    while (!(TWCR & TWI_INTERRUPT)); // Wait for completion
    status = TWSR & TW_STATUS_MASK; // Read status
    if (status != TW_MR_SLA_ACK) return -2; // Check for ACK

    // Read data bytes
    for (uint16_t i = 0; i < length; i++) {
        if (i < (length - 1)) {
            TWCR = TWI_ENABLE | TWI_INTERRUPT | TWI_ENABLE_ACK; // ACK after receive (not last byte)
        } else {
            TWCR = TWI_ENABLE | TWI_INTERRUPT; // NACK after last byte
        }
        while (!(TWCR & TWI_INTERRUPT)); // Wait for completion
        status = TWSR & TW_STATUS_MASK; // Read status
        if (i < (length - 1) && status != TW_MR_DATA_ACK) return -3; // Check for ACK
        if (i == (length - 1) && status != TW_MR_DATA_NACK) return -4; // Check for NACK on last byte
        data[i] = TWDR; // Store received byte
    }

    // Send STOP condition
    TWCR = TWI_ENABLE | TWI_STOP | TWI_INTERRUPT; // Request STOP
    return 0;
}
