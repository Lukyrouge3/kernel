#ifndef IO_SERIAL_H
#define IO_SERIAL_H

#include "io/io.h"
#define COM1 0x3F8

void serial_write(const char *str);
void serial_putc(const char c);

#endif