#ifndef SPI_W25Q128FV_H        // Prevent multiple inclusion of this header file
#define SPI_W25Q128FV_H

#include "drivers/spi.h"


void wait_for_clearance(SPI_Handler_t *pToSPIx);

uint8_t check_write_clearance(SPI_Handler_t *pToSPIx);


/**
 * @brief Fetches data from flash and store it at the pToDataRetreived location
 *
 * @param pToSPIx Pointer to SPI handler.
 * @param blockAddress pointer to where the address of the block is stored. This is a 3 byte address
 * @return uint8_t The byte received during transmission.
 */
uint8_t flash_read_data(SPI_Handler_t *pToSPIx, uint8_t *blockAddress, uint32_t length, uint8_t *pToDataRetreived);
void flash_write_data(SPI_Handler_t *pToSPIx, uint8_t *blockAddress, uint32_t length, uint8_t *pToDataToWrite);
void flash_erase(SPI_Handler_t *pToSPIx, uint8_t *blockAddress, uint32_t length);



#endif // SPI_W25Q128FV_H