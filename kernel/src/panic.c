#include "panic.h"
#include "cpu_utils/cpu_utils.h"
#include "io/printf/printf.h"
#include "io/serial.h"
#include <stdbool.h>

__attribute__((noreturn)) void cpu_halt_forever(void) {
    __asm__ volatile("cli");
    for (;;) {
        __asm__ volatile("hlt");
    }
}
__attribute__((noreturn)) void panic_impl(const char *file, int line, const char *func,
                                          const char *format, ...) {
    static volatile bool panicking = false;

    if (panicking)
        cpu_halt_forever();
    panicking = true;

    serial_printf("\n\n*** KERNEL PANIC ***\n");
    serial_printf("At %s:%d in %s()\n", file, line, func);

    cpu_registers_t regs;
    get_cpu_registers(&regs);
    print_cpu_registers(&regs);

    if (format) {
        va_list args;
        va_start(args, format);
        printf(format, serial_putc, &args);
        va_end(args);
        serial_printf("\n");
    }

    cpu_halt_forever();
}