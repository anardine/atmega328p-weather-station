#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include "drivers/atmega328p.h"
#include "drivers/gpio.h"


typedef struct {
    uint8_t mode;         // Master (1) or Slave (0)
    uint8_t dataOrder;    // MSB first (0) or LSB first (1)
    uint8_t clockPolarity; // Clock Polarity: Idle Low (0) or Idle High (1)
    uint8_t clockPhase;    // Clock Phase: Sample Leading (0) or Trailing (1)
    uint8_t clockRate;     // SPI Clock Rate (0-3: Fosc/4, Fosc/16, Fosc/64, Fosc/128)
    uint8_t doubleSpeed;   // Double SPI Speed Bit (0: normal, 1: double)
    uint8_t interruptEnable; // Enable SPI interrupt (0: disable, 1: enable)
} SPI_config_t;


typedef struct 
{
    SPI_TypeDef *pToSPIx;
    SPI_config_t SPIConfig;
} SPI_Handler_t;


/**
 * @brief Initializes the SPI peripheral with the specified configuration.
 *
 * @param spiHandler Pointer to an SPI_Handler_t structure containing configuration and peripheral pointer.
 */
void spi_init(SPI_Handler_t *spiHandler);

/**
 * @brief Writes a byte of data over SPI and returns the received byte.
 *
 * @param spiHandler Pointer to SPI handler.
 * @param data Byte to send.
 * @return uint8_t The byte received during transmission.
 */
uint8_t spi_write(SPI_Handler_t *spiHandler, uint8_t data);

/**
 * @brief Reads a byte of data from SPI (by sending dummy data).
 *
 * @param spiHandler Pointer to SPI handler.
 * @return uint8_t The byte received from SPI.
 */
uint8_t spi_read(SPI_Handler_t *spiHandler);

#endif