#if USE_FLASH
#include "aux/spi_w25q128fv.h"

void wait_for_clearance(SPI_Handler_t *pToSPIx) {
        uint8_t status;
    do {
        uint8_t address = 0x05;
        spi_write(pToSPIx,&address, 1); // Read Status Register-1 command
        status = spi_read(pToSPIx);
    } while (status & 0x01); // WIP bit is set if busy
}

uint8_t check_write_clearance(SPI_Handler_t *pToSPIx) {
    uint8_t status;
    uint8_t address = 0x05;
        spi_write(pToSPIx, &address, 1); // Read Status Register-1 command
        status = spi_read(pToSPIx);
    
        if(status & 2) {
            return 0;
        } else {
            return -1;
        }
}


uint8_t flash_read_data(SPI_Handler_t *pToSPIx, uint8_t *blockAddress, uint32_t length, uint8_t *pToDataRetreived) {
    
    // Set CS low to start communication
    DDRB |= (1 << 2);
    PORTB &= ~(1 << 2);

    wait_for_clearance(pToSPIx);
    uint8_t address = 0x03;
    spi_write(pToSPIx, &address, 1); // sends the read command
    spi_write(pToSPIx, blockAddress, 3); // send the block address where the data is stored

    for(uint32_t i=0; i<length; i++) {
    
        pToDataRetreived[i] = spi_read(pToSPIx); // reads the data after the address is sent using a dummy byte

    }

    // Wait for read to finish
    wait_for_clearance(pToSPIx);

    PORTB |= (1 << 2); // set CS to high to stop communication

    return 0; 

}


void flash_write_data(SPI_Handler_t *pToSPIx, uint8_t *blockAddress, uint32_t length, uint8_t *pToDataToWrite) {

    // set CC to low on pin PB2 by setting the pin as an output pin
    DDRB |= (1 << 2);
    DDRB &= ~(1 << 2);

    wait_for_clearance(pToSPIx); // device ready

    if(check_write_clearance(pToSPIx)) { // device write enabled if 0
        uint8_t address = 0x06;
        spi_write(pToSPIx, &address, 1); // Write Enable instruction

    } 

       // set CS to high and low again given the necessity for the W25Q128FV to process
        DDRB ^= (1 << 2);
        DDRB ^= (1 << 2);

        uint8_t address = 0x02;
        // Page Program command (0x02)
        spi_write(pToSPIx, &address, 1);
        // Send 3-byte address
        spi_write(pToSPIx, blockAddress, 3);
        // Write data bytes
        for(uint32_t i = 0; i < length; i++) {
            spi_write(pToSPIx, &pToDataToWrite[i], 1);
        }
        // Wait for write to finish
        wait_for_clearance(pToSPIx);

    DDRB |= (1 << 2); // set CS to high to stop communication
}

void flash_erase(SPI_Handler_t *pToSPIx, uint8_t *blockAddress, uint32_t length, uint8_t eraseMode) {

    // set CC to low on pin PB2 by setting the pin as an output pin
    DDRB |= (1 << 2);
    DDRB &= ~(1 << 2);

    wait_for_clearance(pToSPIx); // device ready

    if(check_write_clearance(pToSPIx)) { // device write enabled
        uint8_t address = 0x06;
        spi_write(pToSPIx, &address, 1); // Write Enable instruction

    }
    // set CS to high and low again given the necessity for the W25Q128FV to process
    PORTB ^= (1 << 2);
    PORTB ^= (1 << 2);

    // Erase logic: Use Sector Erase (0x20) or Block Erase (0xD8) or Chip Erase (0xC7)
    // Here, we'll use Sector Erase (0x20) for the address provided
    // You can change to 0xD8 for 64KB block erase or 0xC7 for chip erase

    //Chip Erase command (0xC7) REVIEW CODE
    spi_write(pToSPIx, &eraseMode, 1); // Send erase command
    //spi_write(pToSPIx, blockAddress, 3); // Send 3-byte address only for sector or block erase

    PORTB |= (1 << 2); // set CS to high. Setting CS high is a must for completing the erasing cycle.
}
#endif