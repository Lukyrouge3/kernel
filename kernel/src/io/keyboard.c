#include "io/keyboard.h"
#include "cpu_utils/cpu_utils.h"
#include "io/printf/printf.h"
#include "stdlib.h"
#include "io/io.h"

void keyboard_handler_c(void) {
    unsigned char scancode = inb(KEYBOARD_CTRL_DATA);
    if (scancode & 0x80)
        return; // Key released, ignore
    if (scancode_map[scancode] == '\n')
    {
        newline_vga();
        return;
    }
    if (isprint(scancode_map[scancode]))
        put_char_vga(scancode_map[scancode]);
}

void __attribute__((naked)) irq1_handler(void) {
    __asm__ volatile("pusha\n"
                     "call keyboard_handler_c\n"
                     "popa\n"
                     "movb $0x20, %al\n"
                     "outb %al, $0x20\n"
                     "iret");
}