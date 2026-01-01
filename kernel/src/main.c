#include "io/printf/printf.h"
#include <stdint.h>

void _start(void) __attribute__((section(".init")));

// cppcheck-suppress unusedFunction
void _start(void) {
    serial_init_com1();

    serial_printf("%s %d %X\n", "The answer is", 42, 0x2A);

    for (;;) {
    }
}