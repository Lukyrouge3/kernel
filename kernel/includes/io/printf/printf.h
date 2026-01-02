#ifndef IO_PRINTF_PRINTF_H
#define IO_PRINTF_PRINTF_H
#include <stdarg.h>

typedef void (*putc_fn)(char c);

void serial_printf(const char *format, ...);
void vga_printf(const char *format, ...);
void printf(const char *format, putc_fn putc, va_list *args);
#endif