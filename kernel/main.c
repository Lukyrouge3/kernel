#include <stdint.h>

static volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

void _start(void) {
    const char* s = "hello from c in pmode";
    for (int i = 0; s[i]; i++) {
        VGA[i] = vga_entry(s[i], 0x0F); // white on black
    }

    for (;;) { __asm__ volatile ("hlt"); }
}