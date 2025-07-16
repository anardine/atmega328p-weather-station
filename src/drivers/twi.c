
#include "drivers/twi.h"
#include "drivers/atmega328p.h"

void twi_init(TWI_handler_t *twiHandler) {
    // Set SCL frequency (default to 100kHz if not specified)
    uint32_t scl_freq = (twiHandler->TWIConfig.speed == 0) ? 100 : twiHandler->TWIConfig.speed;
    // Calculate TWBR value for desired SCL frequency
    uint32_t twbr_val = ((F_CPU / (scl_freq * 1000UL)) - 16) / 2;
    twiHandler->pTWIx->twbr = (uint8_t)twbr_val; // Set bit rate register
    twiHandler->pTWIx->twsr &= ~((1 << 0) | (1 << 1)); // Clear prescaler bits (TWPS0, TWPS1)

    // Set own address if in slave mode
    if (twiHandler->TWIConfig.ownAddress) {
        twiHandler->pTWIx->twar = (twiHandler->TWIConfig.ownAddress << 1); // Set slave address
    }

    GPIOC.port |= (1 << 4) | (1 << 5); // enable pull up resistors for twi communication on PC4 and PC5

    // Enable TWI peripheral
    twiHandler->pTWIx->twcr = TWI_ENABLE; //When TWI is enabled, the hardware automatically takes control of PC4/PC5 and makes them behave as open-drain
}


int twi_write(TWI_handler_t *twiHandler, uint8_t address, const uint8_t *data, uint16_t length) {
    // Send START condition
    twiHandler->pTWIx->twcr = TWI_START | TWI_ENABLE | TWI_INTERRUPT; // Request START
    while (!(twiHandler->pTWIx->twcr & TWI_INTERRUPT)); // Wait for START to complete
    uint8_t status = twiHandler->pTWIx->twsr & TW_STATUS_MASK; // Read status
    if (status != TW_START_VAL) return -1; // Check for successful START

    // Send SLA+W (address + write bit)
    twiHandler->pTWIx->twdr = (address << 1); // Set address and write bit
    twiHandler->pTWIx->twcr = TWI_ENABLE | TWI_INTERRUPT; // Start transmission
    while (!(twiHandler->pTWIx->twcr & TWI_INTERRUPT)); // Wait for completion
    status = twiHandler->pTWIx->twsr & TW_STATUS_MASK; // Read status
    if (status != TW_MT_SLA_ACK) return -2; // Check for ACK

    // Send data bytes
    for (uint16_t i = 0; i < length; i++) {
        twiHandler->pTWIx->twdr = data[i]; // Load data byte
        twiHandler->pTWIx->twcr = TWI_ENABLE | TWI_INTERRUPT; // Start transmission
        while (!(twiHandler->pTWIx->twcr & TWI_INTERRUPT)); // Wait for completion
        status = twiHandler->pTWIx->twsr & TW_STATUS_MASK; // Read status
        if (status != TW_MT_DATA_ACK) return -3; // Check for ACK
    }

    // Send STOP condition
    twiHandler->pTWIx->twcr = TWI_ENABLE | TWI_STOP | TWI_INTERRUPT; // Request STOP
    return 0;
}


int twi_read(TWI_handler_t *twiHandler, uint8_t address, uint8_t *data, uint16_t length) {
    // Send START condition
    twiHandler->pTWIx->twcr = TWI_START | TWI_ENABLE | TWI_INTERRUPT; // Request START
    while (!(twiHandler->pTWIx->twcr & TWI_INTERRUPT)); // Wait for START to complete
    uint8_t status = twiHandler->pTWIx->twsr & TW_STATUS_MASK; // Read status
    if (status != TW_START_VAL) return -1; // Check for successful START

    // Send SLA+R (address + read bit)
    twiHandler->pTWIx->twdr = (address << 1) | 1; // Set address and read bit
    twiHandler->pTWIx->twcr = TWI_ENABLE | TWI_INTERRUPT; // Start transmission
    while (!(twiHandler->pTWIx->twcr & TWI_INTERRUPT)); // Wait for completion
    status = twiHandler->pTWIx->twsr & TW_STATUS_MASK; // Read status
    if (status != TW_MR_SLA_ACK) return -2; // Check for ACK

    // Read data bytes
    for (uint16_t i = 0; i < length; i++) {
        if (i < (length - 1)) {
            twiHandler->pTWIx->twcr = TWI_ENABLE | TWI_INTERRUPT | TWI_ENABLE_ACK; // ACK after receive (not last byte)
        } else {
            twiHandler->pTWIx->twcr = TWI_ENABLE | TWI_INTERRUPT; // NACK after last byte
        }
        while (!(twiHandler->pTWIx->twcr & TWI_INTERRUPT)); // Wait for completion
        status = twiHandler->pTWIx->twsr & TW_STATUS_MASK; // Read status
        if (i < (length - 1) && status != TW_MR_DATA_ACK) return -3; // Check for ACK
        if (i == (length - 1) && status != TW_MR_DATA_NACK) return -4; // Check for NACK on last byte
        data[i] = twiHandler->pTWIx->twdr; // Store received byte
    }

    // Send STOP condition
    twiHandler->pTWIx->twcr = TWI_ENABLE | TWI_STOP | TWI_INTERRUPT; // Request STOP
    return 0;
}
