#include "drivers/timer.h"

// Timer1 initialization. Initializing this function enables the timer and global interrupts.
void timer1_init(TIMER_Handler_t *timerHandler) {

    // reset the timer to zero
    TCNT1 = 0;
    TCCR1A = 0;

        // Set prescaler (CS12:CS10)
    switch (timerHandler->config.prescaler) {
        case 1:
            TCCR1B |= (1 << 0); // CS10
            break;
        case 8:
            TCCR1B |= (1 << 1); // CS11
            break;
        case 64:
            TCCR1B |= (1 << 0) | (1 << 1); // CS10 | CS11
            break;
        case 256:
            TCCR1B |= (1 << 2); // CS12
            break;
        case 1024:
            TCCR1B |= (1 << 0) | (1 << 2); // CS10 | CS12
            break;
        default:
            TCCR1B |= (1 << 0); // Default to CS10
            break;
    }

    // Enable • Bit 0 – TOIE1: Timer/Counter1, Overflow Interrupt Enable
    TIMSK1 |= (1 << 0); // TOIE1

    sei(); //enable global interrupts
}

void timer1_stop(TIMER_Handler_t *timerHandler) {
    // Clear prescaler bits to stop timer
    TCCR1B &= ~((1 << 0) | (1 << 1) | (1 << 2));
}

void timer1_reset(TIMER_Handler_t *timerHandler) {

    // reset the timer to zero
    TCNT1 = 0;

}
