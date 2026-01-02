#include "io/keyboard.h"
#include "cpu_utils/cpu_utils.h"
#include "io/io.h"
#include "stdlib.h"

static const char scancode_map[128] = {
    0,    27,  '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', '-', '=',  '\b', // Backspace
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',  '[', ']', '\n',       // Enter
    0,                                                                             // Ctrl
    'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,   '\\', 'z',
    'x',  'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,   '*',  0,   ' ', // Space
    // rest left 0
};

void keyboard_handler_c(void) {
    unsigned char scancode = inb(KEYBOARD_CTRL_DATA);
    if (scancode & 0x80)
        return; // Key released, ignore
    if (scancode_map[scancode] == '\n') {
        vga_newline();
        return;
    }
    if (isprint(scancode_map[scancode]))
        vga_putc(scancode_map[scancode]);
}

void __attribute__((naked)) irq1_handler(void) {
    __asm__ volatile("pushal\n"
                     "call keyboard_handler_c\n"
                     "popal\n"
                     "movb $0x20, %al\n"
                     "outb %al, $0x20\n"
                     "iret");
}