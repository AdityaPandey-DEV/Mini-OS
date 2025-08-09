#include "timer.h"
#include "lib.h"

static volatile uint64_t uptime_ms = 0;

void timer_init(uint32_t freq) {
    // PIT at 1193180 Hz
    uint32_t divisor = 1193180 / (freq ? freq : 100);
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

// called from irq0 handler via __lib_tick_inc; we just derive uptime on read
uint64_t timer_uptime_ms(void) {
    // approximate from ticks at 100 Hz
    extern uint32_t ticks(void);
    return (uint64_t)ticks() * 10ULL;
}

