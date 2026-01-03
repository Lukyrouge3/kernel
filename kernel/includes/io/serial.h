#pragma once

#define COM1 0x3F8

void serial_write(const char *str);
void serial_putc(const char c);
void serial_init_com1(void);
int serial_can_transmit(void);
