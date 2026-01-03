#pragma once

#include <stdint.h>

// PIT (Programmable Interval Timer) constants
#define PIT_BASE_FREQUENCY 1193182 // PIT oscillator frequency in Hz
#define TIMER_FREQUENCY_HZ 1000    // Desired timer frequency (1000 Hz = 1ms per tick)
#define TIMER_INTERVAL_NS (1000000000 / TIMER_FREQUENCY_HZ) // Interval in nanoseconds

void timer_init(void);
uint64_t timer_ticks(void);
void sleep(uint32_t ms);
void timer_interrupt_handler(void);