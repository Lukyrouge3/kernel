#include <stdint.h>

void _start(void) __attribute__((section(".init")));

#define VGA_HEIGHT 80
#define VGA_WIDTH 25

static volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

void    clearScreen(void)
{
    for (int i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++)
    {
        VGA[i] = 0x0;
    }
}

void    kprint(const char* string, uint8_t color)
{
    for (int i = 0; string[i]; i++) {
        VGA[i] = vga_entry(string[i], color); // white on black
    }
}

void _start(void) {
    const char* s = "hello from c in pmode";
    clearScreen();
    kprint(s, 0x0F);

    for (;;) {  }
}