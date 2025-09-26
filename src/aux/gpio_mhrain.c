#include "aux/gpio_mhrain.h"


uint8_t read_rain() {
    uint8_t state = 0;
    state = ((PINC >> PINC2) & 0x01);
    // return the value
    return state;
}