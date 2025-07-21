
#include "aux/spi_w25q128fv.h"


uint8_t flash_read_data(SPI_Handler_t *pToSPIx, uint8_t *blockAddress, uint32_t length, uint8_t *pToDataRetreived) {

    // set CC to low on pin PB2 by setting the pin as an output pin
    GPIOB->ddr |= (1 << 2);
    GPIOB->port &= ~(1 << 2);
    
    spi_write(pToSPIx, 0x03, 1); // sends the read command
    spi_write(pToSPIx, blockAddress, 3); // send the block address where the data is stored

    for(uint32_t i=0; i<length; i++) {
    
        pToDataRetreived[i] = spi_read(pToSPIx); // reads the data after the address is sent using a dummy byte

    }

    GPIOB->port |= (1 << 2); // set CS to high to stop communication

    return 0; 

}


void flash_write_data(void) {




}