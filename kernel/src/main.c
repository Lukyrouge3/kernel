#include "io/printf/printf.h"
#include <stdint.h>

void _start(void) __attribute__((section(".init")));

static volatile uint16_t *const VGA = (uint16_t *)0xB8000;
static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

// cppcheck-suppress unusedFunction
void _start(void) {
    serial_init_com1();

    serial_printf("%s %d %X\n", "The answer is", 42, 0x2A);

    for (;;) {
    }
}