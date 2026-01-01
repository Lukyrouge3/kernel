#include <stdint.h>

void _start(void) __attribute__((section(".init")));

#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define VGA_ENTRIES (VGA_WIDTH * VGA_HEIGHT)

#define TEXT_COLOR 0x0F

#define VGA_CRT_CTRL_ADDR 0x3D4
#define VGA_CRT_CTRL_DATA 0x3D5

#define  VGA_CURSOR_LOC_HIGH 0x0F
#define  VGA_CURSOR_LOC_LOW 0x0E

static volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

uint16_t get_cursor_pos(void) {  
    outb(VGA_CRT_CTRL_ADDR, VGA_CURSOR_LOC_HIGH);
    uint16_t pos = inb(VGA_CRT_CTRL_DATA);
    outb(VGA_CRT_CTRL_ADDR, VGA_CURSOR_LOC_LOW);
    pos |= (uint16_t)inb(VGA_CRT_CTRL_DATA) << 8;
    return pos;
}

void set_cursor_pos(uint16_t pos) {
    outb(VGA_CRT_CTRL_ADDR, VGA_CURSOR_LOC_HIGH);
    outb(VGA_CRT_CTRL_DATA, (uint8_t)(pos & 0xFF));
    outb(VGA_CRT_CTRL_ADDR, VGA_CURSOR_LOC_LOW);
    outb(VGA_CRT_CTRL_DATA, (uint8_t)((pos >> 8) & 0xFF));
}

void    clear_screen(void)
{
    for (int i = 0; i < VGA_ENTRIES; i++)
    {
        VGA[i] = vga_entry(' ', (uint8_t)TEXT_COLOR);
    }
    set_cursor_pos(0);
}

void    kprint(const char* string)
{
    uint16_t pos = get_cursor_pos();

    for (int i = 0; string[i] && pos < VGA_ENTRIES; i++) {
        char c = string[i];

        if (c == '\n') {
            pos = (pos / VGA_WIDTH + 1) * VGA_WIDTH;
            continue;
        }

        VGA[pos++] = vga_entry(c, (uint8_t)TEXT_COLOR);
    }

    if (pos >= VGA_ENTRIES)
        pos = VGA_ENTRIES - 1;
    set_cursor_pos(pos);
}

void    scroll(void)
{
    uint16_t pos = get_cursor_pos();

    for (int i = 0; i < pos; i++)
    {
        if (i + VGA_WIDTH > pos)
        {
            VGA[i] = vga_entry(' ', TEXT_COLOR);
            continue;
        }
        VGA[i] = vga_entry(VGA[i + VGA_WIDTH], TEXT_COLOR);
    }
    set_cursor_pos(pos - VGA_WIDTH);
}

void _start(void) {
    clear_screen();
    kprint("Hello from kernel!\nThis is a new line that updates the cursor's position accordingly.");
    scroll();
    while(1)
        ;
}