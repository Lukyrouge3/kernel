#include "timer.h"
#include "cpu_utils/cpu_utils.h"
#include "io/io.h"

// PIT ports
#define PIT_CHANNEL0_DATA 0x40
#define PIT_COMMAND 0x43

static volatile uint64_t ticks = 0;

void timer_init(void) {
    // Calculate the divisor for desired frequency
    uint16_t divisor = PIT_BASE_FREQUENCY / TIMER_FREQUENCY_HZ;

    // Send command byte: Channel 0, lobyte/hibyte, rate generator mode
    outb(PIT_COMMAND, 0x36);

    // Send divisor (low byte first, then high byte)
    outb(PIT_CHANNEL0_DATA, divisor & 0xFF);
    outb(PIT_CHANNEL0_DATA, (divisor >> 8) & 0xFF);
}

void timer_interrupt_handler() {
    ticks++;
}

uint64_t timer_ticks(void) {
    return ticks;
}

void usleep(uint32_t usec) {
    uint64_t start_ticks = timer_ticks();
    uint64_t wait_ticks =
        (usec * 1000) / TIMER_INTERVAL_NS; // Assuming TIMER_INTERVAL_NS is defined elsewhere

    while ((timer_ticks() - start_ticks) < wait_ticks) {
        // halt
        // halt_cpu();
    }
}