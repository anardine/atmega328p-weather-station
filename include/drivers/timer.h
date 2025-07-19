#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "drivers/atmega328p.h"

// Timer configuration structure
typedef struct {
    uint32_t interval_ms;   // Interval in milliseconds
    uint8_t prescaler;      // Prescaler value (1, 8, 64, 256, 1024)
    uint8_t mode;           // 0: Normal, 1: CTC
} TIMER_config_t;

// Timer handler structure
typedef struct {
    TIMER16_TypeDef *pTIMERx;   // Pointer to timer peripheral (Timer1)
    TIMER_config_t config;
} TIMER_Handler_t;

// Function declarations
void timer1_init(TIMER_Handler_t *timerHandler);
void timer1_start(TIMER_Handler_t *timerHandler);
void timer1_stop(TIMER_Handler_t *timerHandler);
void timer1_set_callback(void (*callback)(void));

#endif // TIMER_H
