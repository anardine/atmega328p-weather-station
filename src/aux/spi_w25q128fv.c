
#include "aux/spi_w25q128fv.h"

void wait_for_clearance(SPI_Handler_t *pToSPIx) {
        uint8_t status;
    do {
        spi_write(pToSPIx, 0x05, 1); // Read Status Register-1 command
        status = spi_read(pToSPIx);
    } while (status & 0x01); // WIP bit is set if busy
}

uint8_t check_write_clearance(SPI_Handler_t *pToSPIx) {
    uint8_t status;

        spi_write(pToSPIx, 0x05, 1); // Read Status Register-1 command
        status = spi_read(pToSPIx);
    
        if(status & 2) {
            return 0;
        } else {
            return -1;
        }
}


uint8_t flash_read_data(SPI_Handler_t *pToSPIx, uint8_t *blockAddress, uint32_t length, uint8_t *pToDataRetreived) {
    
    // Set CS low to start communication
    GPIOB->ddr |= (1 << 2);
    GPIOB->port &= ~(1 << 2);

    wait_for_clearance(pToSPIx);

    spi_write(pToSPIx, 0x03, 1); // sends the read command
    spi_write(pToSPIx, blockAddress, 3); // send the block address where the data is stored

    for(uint32_t i=0; i<length; i++) {
    
        pToDataRetreived[i] = spi_read(pToSPIx); // reads the data after the address is sent using a dummy byte

    }

    // Wait for read to finish
    wait_for_clearance(pToSPIx);

    GPIOB->port |= (1 << 2); // set CS to high to stop communication

    return 0; 

}


void flash_write_data(SPI_Handler_t *pToSPIx, uint8_t *blockAddress, uint32_t length, uint8_t *pToDataToWrite) {

    // set CC to low on pin PB2 by setting the pin as an output pin
    GPIOB->ddr |= (1 << 2);
    GPIOB->port &= ~(1 << 2);

    wait_for_clearance(pToSPIx); // device ready

    if(check_write_clearance(pToSPIx)) { // device write enabled if 0

        spi_write(pToSPIx, 0x06, 1); // Write Enable instruction

    } 
        
        // Page Program command (0x02)
        spi_write(pToSPIx, 0x02, 1);
        // Send 3-byte address
        spi_write(pToSPIx, blockAddress, 3);
        // Write data bytes
        for(uint32_t i = 0; i < length; i++) {
            spi_write(pToSPIx, &pToDataToWrite[i], 1);
        }
        // Wait for write to finish
        wait_for_clearance(pToSPIx);

    GPIOB->port |= (1 << 2); // set CS to high to stop communication
}

void flash_erase(SPI_Handler_t *pToSPIx, uint8_t *blockAddress, uint32_t length, uint8_t eraseMode) {

    // set CC to low on pin PB2 by setting the pin as an output pin
    GPIOB->ddr |= (1 << 2);
    GPIOB->port &= ~(1 << 2);

    wait_for_clearance(pToSPIx); // device ready

    if(check_write_clearance(pToSPIx)) { // device write enabled

        spi_write(pToSPIx, 0x06, 1); // Write Enable instruction

    }

    // Erase logic: Use Sector Erase (0x20) or Block Erase (0xD8) or Chip Erase (0xC7)
    // Here, we'll use Sector Erase (0x20) for the address provided
    // You can change to 0xD8 for 64KB block erase or 0xC7 for chip erase

    // Sector Erase command (0x20)
    spi_write(pToSPIx, eraseMode, 1); // Send erase command
    spi_write(pToSPIx, blockAddress, 3); // Send 3-byte address

    GPIOB->port |= (1 << 2); // set CS to high. Setting CS high is a must for completing the erasing cycle.
}