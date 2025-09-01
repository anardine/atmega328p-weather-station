#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "drivers/atmega328p.h"

// Timer configuration structure
typedef struct {
    uint32_t prescaler;      // Prescaler value (1, 8, 64, 256, 1024)
} TIMER_config_t;

// Timer handler structure
typedef struct {
    TIMER16_TypeDef *pTIMERx;   // Pointer to timer peripheral (Timer1)
    TIMER_config_t config;
} TIMER_Handler_t;

// Function declarations
void timer1_init(TIMER_Handler_t *timerHandler);
void timer1_stop(TIMER_Handler_t *timerHandler);
void timer1_reset(TIMER_Handler_t *timerHandler);

#endif // TIMER_H
