#include "timer.h"
#include "lib.h"

void timer_init(uint32_t freq) {
    // PIT at 1193180 Hz
    uint32_t divisor = 1193180 / (freq ? freq : 100);
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

// Uptime in milliseconds as 32-bit (wraps after ~49.7 days)
uint32_t timer_uptime_ms(void) {
    extern uint32_t ticks(void);
    return ticks() * 10U;
}

