#ifndef IO_PRINTF_PRINTF_H
#define IO_PRINTF_PRINTF_H
#include "io/serial.h"
#include "stdlib.h"
#include <stdarg.h>
#include <stdbool.h>

typedef void (*putc_fn)(char c);

void serial_printf(const char *format, ...);
void vga_printf(const char *format, ...);
#endif