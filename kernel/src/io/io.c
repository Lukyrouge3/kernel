#include "io/io.h"

void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void io_wait(void) {
        // Old-school tiny delay (port 0x80 is historically unused)
    __asm__ __volatile__ ("outb %%al, $0x80" : : "a"(0));
}