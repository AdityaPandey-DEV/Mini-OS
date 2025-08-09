#pragma once
#include <stdint.h>

void timer_init(uint32_t freq);
uint64_t timer_uptime_ms(void);

