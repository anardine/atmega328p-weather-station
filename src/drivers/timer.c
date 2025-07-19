#include "drivers/timer.h"

static void (*timer1_callback)(void) = 0;

// Timer1 initialization for CTC mode
void timer1_init(TIMER_Handler_t *timerHandler) {
    // Set CTC mode (Clear Timer on Compare Match)
    timerHandler->pTIMERx->tccrb |= (1 << 3); // WGM12
    timerHandler->pTIMERx->tccra &= ~((1 << 0) | (1 << 1)); // WGM10, WGM11

    // Calculate OCR1A for desired interval
    uint32_t ticks = (F_CPU / timerHandler->config.prescaler) * timerHandler->config.interval_ms / 1000;
    timerHandler->pTIMERx->ocra = (uint16_t)(ticks - 1);

    // Enable Output Compare A Match Interrupt
    timerHandler->pTIMERx->timsk |= (1 << 1); // OCIE1A
}

void timer1_start(TIMER_Handler_t *timerHandler) {
    // Set prescaler (CS12:CS10)
    switch (timerHandler->config.prescaler) {
        case 1:
            timerHandler->pTIMERx->tccrb |= (1 << 0); // CS10
            break;
        case 8:
            timerHandler->pTIMERx->tccrb |= (1 << 1); // CS11
            break;
        case 64:
            timerHandler->pTIMERx->tccrb |= (1 << 0) | (1 << 1); // CS10 | CS11
            break;
        case 256:
            timerHandler->pTIMERx->tccrb |= (1 << 2); // CS12
            break;
        case 1024:
            timerHandler->pTIMERx->tccrb |= (1 << 0) | (1 << 2); // CS10 | CS12
            break;
        default:
            timerHandler->pTIMERx->tccrb |= (1 << 0); // Default to CS10
            break;
    }
}

void timer1_stop(TIMER_Handler_t *timerHandler) {
    // Clear prescaler bits to stop timer
    timerHandler->pTIMERx->tccrb &= ~((1 << 0) | (1 << 1) | (1 << 2));
}

void timer1_set_callback(void (*callback)(void)) {
    timer1_callback = callback;
}

// This function should be called from the Timer1 Compare Match ISR
void timer1_isr_handler(void) {
    if (timer1_callback) {
        timer1_callback();
    }
}
