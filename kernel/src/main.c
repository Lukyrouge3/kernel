#include <stdint.h>
#include "io/serial.h"

void _start(void) __attribute__((section(".init")));

static volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

// cppcheck-suppress unusedFunction
void _start(void) {
    serial_init_com1();

    const char* s = "hello from c in pmode";
    for (int i = 0; s[i]; i++) {
        VGA[i] = vga_entry(s[i], 0x0F); // white on black
    }

    serial_write("Hello World\n");

    for (;;) {  }
}