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

/**
 * Timer interrupt callback.
 *
 * This function is invoked from the hardware timer IRQ handler and thus runs
 * in interrupt context. It must remain interrupt-safe and perform only
 * minimal, non-blocking work (e.g., updating simple counters).
 */
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

// Busy-wait until the specified number of timer ticks has elapsed.
// Note: The parameter is named 'ms' for historical reasons. If TIMER_FREQUENCY_HZ
//       is configured to 1000 Hz, one tick is approximately 1 ms; otherwise the
//       duration is in timer ticks, not strict milliseconds.
void sleep(uint32_t ms) {
    uint64_t start_ticks = timer_ticks();
    uint64_t ticks_to_wait = ((uint64_t)ms * TIMER_FREQUENCY_HZ) / 1000;
    while ((timer_ticks() - start_ticks) < ticks_to_wait) {
        halt_cpu(); // Halt CPU to save power while waiting
    }
}