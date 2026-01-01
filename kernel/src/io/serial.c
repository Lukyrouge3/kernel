#include "io/serial.h"

void serial_init_com1(void) {
    outb(COM1 + 1, 0x00); // Disable all interrupts
    outb(COM1 + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(COM1 + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(COM1 + 1, 0x00); //                  (hi byte)
    outb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(COM1 + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set
    io_wait();
}

static int serial_can_transmit(void) {
    return (inb(COM1 + 5) & (1 << 5)) != 0; // LSR bit 5 = THRE
}

void serial_write(const char *str) {
    for (; *str; str++) {
        if (*str == '\n')
            serial_putc('\r'); // common convention
        serial_putc(*str);
    }
}

void serial_putc(const char c) {
    while (!serial_can_transmit()) {
    };
    outb(COM1, c);
}