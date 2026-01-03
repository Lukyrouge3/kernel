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
    // Disable interrupts to ensure atomic read of 64-bit value on 32-bit x86
    uint32_t flags = read_eflags();
    disable_interrupts();
    uint64_t result = ticks;
    write_eflags(flags);
    return result;
}

void sleep(uint32_t ms) {
    uint64_t start_ticks = timer_ticks();
    while ((timer_ticks() - start_ticks) < ms) {
        halt_cpu(); // Halt CPU to save power while waiting
    }
}