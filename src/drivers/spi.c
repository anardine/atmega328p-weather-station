#if USE_FLASH

#include "drivers/spi.h"
#include <stddef.h>

/**
 * @brief Initializes the SPI peripheral with the specified configuration.
 *
 * @param spiHandler Pointer to an SPI_Handler_t structure containing configuration and peripheral pointer.
 */
void spi_init(SPI_Handler_t *spiHandler)
{   
    // Configure SPI pins as outputs/inputs based on master/slave mode
    if (spiHandler->SPIConfig.mode == SPI_MODE_MASTER) {
        // Master mode: Set MOSI, SCK, and SS as outputs, MISO as input
        DDRB |= (1 << SPI_MOSI_PIN) | (1 << SPI_SCK_PIN) | (1 << SPI_SS_PIN);
        DDRB &= ~(1 << SPI_MISO_PIN);
        
    } else {
        // Slave mode: Set MISO as output, MOSI, SCK, and SS as inputs
        DDRB |= (1 << SPI_MISO_PIN);
        DDRB &= ~((1 << SPI_MOSI_PIN) | (1 << SPI_SCK_PIN) | (1 << SPI_SS_PIN));
    }
    
    // Configure SPCR (SPI Control Register)
    uint8_t spcr = 0;
    
    // Set interrupt enable
    if (spiHandler->SPIConfig.interruptEnable == SPI_INT_ENABLE) {
        spcr |= (1 << SPI_SPIE);
    }
    
    // Set data order
    if (spiHandler->SPIConfig.dataOrder == SPI_DATA_ORDER_LSB) {
        spcr |= (1 << SPI_DORD);
    }
    
    // Set master/slave mode
    if (spiHandler->SPIConfig.mode == SPI_MODE_MASTER) {
        spcr |= (1 << SPI_MSTR);
    }
    
    // Set clock polarity
    if (spiHandler->SPIConfig.clockPolarity == SPI_CLOCK_POL_HIGH) {
        spcr |= (1 << SPI_CPOL);
    }
    
    // Set clock phase
    if (spiHandler->SPIConfig.clockPhase == SPI_CLOCK_PHASE_TRAIL) {
        spcr |= (1 << SPI_CPHA);
    }
    
    // Set clock rate (SPR1:SPR0 bits)
    spcr |= (spiHandler->SPIConfig.clockRate & 0x03);
    
    // Write to SPCR register
        SPCR = spcr;
    
    // Configure SPSR (SPI Status Register) for double speed if needed
    uint8_t spsr = SPSR & 0xFE; // Clear SPI2X bit
    if (spiHandler->SPIConfig.doubleSpeed == SPI_CLOCK_DOUBLE) {
        spsr |= (1 << SPI_SPI2X);
    }
        SPSR = spsr;

    // Enable SPI
    SPCR |= (1 << SPI_SPE);
}

/**
 * @brief Writes a byte of data over SPI and returns the received byte.
 *
 * @param spiHandler Pointer to SPI handler.
 * @param data Byte to send.
 * @return The byte received during transmission.
 */
uint8_t spi_write(SPI_Handler_t *spiHandler, uint8_t *data, uint32_t length)
{
    if (spiHandler == NULL || spiHandler->pToSPIx == NULL) {
        return -1; // Invalid handler
    }

    for(uint32_t i=0; i<length; i++) {
    // Start transmission by writing data to SPDR
    SPDR = data[i];
    
    // Wait for transmission complete (SPIF flag in SPSR)
    while (!(SPSR & (1 << SPI_SPIF)));

    }

    return SPSR;

}

/**
 * @brief Reads a byte of data from SPI (by sending dummy data).
 *
 * @param spiHandler Pointer to SPI handler.
 * @return uint8_t The byte received from SPI.
 */
uint8_t spi_read(SPI_Handler_t *spiHandler)
{
    if (spiHandler == NULL || spiHandler->pToSPIx == NULL) {
        return 0; // Invalid handler
    }
    
    // Send dummy byte (0xFF is commonly used) to generate clock and receive data
    uint8_t dummyData = 0xFF;
    return spi_write(spiHandler, &dummyData, 1);
}

#endif
