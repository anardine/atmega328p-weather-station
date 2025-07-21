
#include "drivers/spi.h"

/**
 * @brief Fetches one byte of data from flash
 *
 * @param pToSPIx Pointer to SPI handler.
 * @param blockAddress pointer to where the address of the block is stored. This is a 3 byte address
 * @return uint8_t The byte received during transmission.
 */
uint8_t flash_read_data(SPI_Handler_t *pToSPIx, uint8_t *blockAddress, uint32_t length, uint8_t *pToDataRetreived);