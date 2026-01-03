#pragma once
#include <stdint.h>

#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define VGA_ENTRIES (VGA_WIDTH * VGA_HEIGHT)

#define TEXT_COLOR 0x0F

#define VGA_CRT_CTRL_ADDR 0x3D4
#define VGA_CRT_CTRL_DATA 0x3D5

#define VGA_CURSOR_LOC_LOW 0x0F
#define VGA_CURSOR_LOC_HIGH 0x0E

uint16_t vga_get_cursor_pos(void);
void vga_set_cursor_pos(uint16_t pos);
void vga_clear_screen(void);
void vga_putc(const char c);
void vga_newline(void);
void vga_scroll(void);
