#include "io/vga.h"
#include "io/io.h"

static volatile uint16_t *const VGA = (uint16_t *)0xB8000;
static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
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

void clear_screen(void) {
    for (int i = 0; i < VGA_ENTRIES; i++) {
        VGA[i] = vga_entry(' ', (uint8_t)TEXT_COLOR);
    }
    set_cursor_pos(0);
}

void put_char_vga(const char c) {
    uint16_t pos = get_cursor_pos();
    if (pos >= VGA_ENTRIES)
    {
        scroll();
        pos = VGA_ENTRIES - VGA_WIDTH;
    }
    VGA[pos++] = vga_entry(c, (uint8_t)TEXT_COLOR);
    set_cursor_pos(pos);
}

void newline_vga(void) {
    uint16_t pos = get_cursor_pos();
    pos += VGA_WIDTH - (pos % VGA_WIDTH);
    if (pos >= VGA_ENTRIES)
    {
        scroll();
        pos = VGA_ENTRIES - VGA_WIDTH;
    }
    set_cursor_pos(pos);
}

void scroll(void) {
    uint16_t pos = get_cursor_pos();

    for (int i = 0; i < pos; i++) {
        if (i + VGA_WIDTH > pos) {
            VGA[i] = vga_entry(' ', TEXT_COLOR);
            continue;
        }
        VGA[i] = vga_entry(VGA[i + VGA_WIDTH], TEXT_COLOR);
    }
    set_cursor_pos(pos - VGA_WIDTH);
}