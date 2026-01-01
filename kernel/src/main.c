#include "io/printf/printf.h"
#include "io/serial.h"
#include "io/vga.h"
#include <stdint.h>

void _start(void) __attribute__((section(".init")));

// cppcheck-suppress unusedFunction
void _start(void) {
    serial_init_com1();

    serial_printf("%s %d %X\n", "The answer is", 42, 0x2A);
    clear_screen();
    kprint("The answer is 42 in decimal and 2A in hexadecimal.\n");
    kprint("Hello, VGA World!\n");
    scroll();

    for (;;) {
    }
}