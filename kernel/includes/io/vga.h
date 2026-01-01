#ifndef IO_VGA_H
#define IO_VGA_H
#include <stdint.h>

#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define VGA_ENTRIES (VGA_WIDTH * VGA_HEIGHT)

#define TEXT_COLOR 0x0F

#define VGA_CRT_CTRL_ADDR 0x3D4
#define VGA_CRT_CTRL_DATA 0x3D5

#define VGA_CURSOR_LOC_HIGH 0x0F
#define VGA_CURSOR_LOC_LOW 0x0E

uint16_t get_cursor_pos(void);
void set_cursor_pos(uint16_t pos);
void clear_screen(void);
void kprint(const char *string);
void scroll(void);
#endif