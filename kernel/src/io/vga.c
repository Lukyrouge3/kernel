#include "io/vga.h"
#include "io/io.h"

static volatile uint16_t *const VGA = (uint16_t *)0xB8000;
static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

uint16_t vga_get_cursor_pos(void) {
    outb(VGA_CRT_CTRL_ADDR, VGA_CURSOR_LOC_HIGH);
    uint16_t pos = inb(VGA_CRT_CTRL_DATA);
    outb(VGA_CRT_CTRL_ADDR, VGA_CURSOR_LOC_LOW);
    pos |= (uint16_t)inb(VGA_CRT_CTRL_DATA) << 8;
    return pos;
}

void vga_set_cursor_pos(uint16_t pos) {
    outb(VGA_CRT_CTRL_ADDR, VGA_CURSOR_LOC_HIGH);
    outb(VGA_CRT_CTRL_DATA, (uint8_t)(pos & 0xFF));
    outb(VGA_CRT_CTRL_ADDR, VGA_CURSOR_LOC_LOW);
    outb(VGA_CRT_CTRL_DATA, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_clear_screen(void) {
    for (int i = 0; i < VGA_ENTRIES; i++) {
        VGA[i] = vga_entry(' ', (uint8_t)TEXT_COLOR);
    }
    vga_set_cursor_pos(0);
}

void vga_putc(const char c) {
    uint16_t pos = vga_get_cursor_pos();
    if (pos >= VGA_ENTRIES) {
        vga_scroll();
        pos = VGA_ENTRIES - VGA_WIDTH;
    }
    VGA[pos++] = vga_entry(c, (uint8_t)TEXT_COLOR);
    vga_set_cursor_pos(pos);
}

void vga_newline(void) {
    uint16_t pos = vga_get_cursor_pos();
    pos += VGA_WIDTH - (pos % VGA_WIDTH);
    if (pos >= VGA_ENTRIES) {
        vga_scroll();
        pos = VGA_ENTRIES - VGA_WIDTH;
    }
    vga_set_cursor_pos(pos);
}

void vga_scroll(void) {
    uint16_t pos = vga_get_cursor_pos();

    for (int i = 0; i < VGA_ENTRIES - VGA_WIDTH; i++) {
        VGA[i] = VGA[i + VGA_WIDTH];
    }

    /* Clear the bottom line */
    for (int i = VGA_ENTRIES - VGA_WIDTH; i < VGA_ENTRIES; i++) {
        VGA[i] = vga_entry(' ', (uint8_t)TEXT_COLOR);
    }
    /* Move cursor up one line safely */
    if (pos >= VGA_WIDTH) {
        pos -= VGA_WIDTH;
    } else {
        pos = 0;
    }
    vga_set_cursor_pos(pos);
}