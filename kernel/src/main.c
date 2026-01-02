#include "io/printf/printf.h"
#include "io/serial.h"
#include "io/vga.h"
#include "cpu_utils/idt.h"
#include "io/pic.h"
#include <stdint.h>

void _start(void) __attribute__((section(".init")));

// cppcheck-suppress unusedFunction
void _start(void) {
    serial_init_com1();
    pic_remap();
    idt_init();
    clear_screen();

    serial_printf("%s %d %X\n", "The answer is", 42, 0x2A);
    for (;;) {
    }
}