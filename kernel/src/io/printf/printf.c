#include "io/printf/printf.h"

static void put_char(va_list args, putc_fn putc) {
    char c = va_arg(args, int);
    putc(c);
}

static void put_int(va_list args, putc_fn putc) {
    int value = va_arg(args, int);
    if (value == 0) {
        putc('0');
        return;
    }

    char buffer[12]; // Enough for 32-bit int
    int index = 0;
    bool is_negative = false;

    if (value < 0) {
        is_negative = true;
        value = -value;
    }

    while (value > 0) {
        buffer[index++] = (value % 10) + '0';
        value /= 10;
    }

    if (is_negative) {
        buffer[index++] = '-';
    }

    for (int i = index - 1; i >= 0; i--) {
        putc(buffer[i]);
    }
}

static void put_hex(va_list args, putc_fn putc, bool uppercase) {
    unsigned int value = va_arg(args, unsigned int);
    char buffer[9]; // Enough for 32-bit hex
    int index = 0;

    if (value == 0) {
        putc('0');
        return;
    }

    while (value > 0) {
        unsigned int digit = value % 16;
        if (digit < 10) {
            buffer[index++] = digit + '0';
        } else {
            buffer[index++] = digit - 10 + 'a';
        }
        value /= 16;
    }

    for (int j = index - 1; j >= 0; j--) {
        putc(uppercase ? toupper(buffer[j]) : buffer[j]);
    }
}

static void printf(const char *format, putc_fn putc, va_list args) {
    for (int i = 0; format[i]; i++) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
            case 'c':
                put_char(args, putc);
                break;
            case 'd':
                put_int(args, putc);
                break;
            case 's': {
                char *str = va_arg(args, char *);
                for (int j = 0; str[j]; j++) {
                    putc(str[j]);
                }
            } break;
            case 'x':
                put_hex(args, putc, false);
                break;
            case 'X':
                put_hex(args, putc, true);
                break;
            default:
                break;
            }
        } else {
            putc(format[i]);
        }
    }
}

void serial_printf(const char *format, ...) {
    va_list args;

    va_start(args, format);
    printf(format, serial_putc, args);
}