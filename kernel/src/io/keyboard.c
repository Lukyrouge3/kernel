#include "io/keyboard.h"
#include "cpu_utils/cpu_utils.h"
#include "io/io.h"
#include "stdlib.h"

static scancode_state_t scancode_state = SCANCODE_NORMAL;

static const char scancode_map[128] = {
    0,    27,  '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', '-', '=',  '\b', // Backspace
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',  '[', ']', '\n',       // Enter
    0,                                                                             // Ctrl
    'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,   '\\', 'z',
    'x',  'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,   '*',  0,   ' ', // Space
    // rest left 0
};

static int scancode_is_break_code(uint8_t scancode) {
    return (scancode & 0x80) != 0;
}

void keyboard_handler_c(void) {
    uint8_t scancode = inb(KEYBOARD_CTRL_DATA);
    if (scancode == 0xE0) {
        scancode_state = SCANCODE_EXTENDED_E0;
        return;
    }
    if (scancode_is_break_code(scancode)) {
        scancode_state = SCANCODE_NORMAL;
        return; // Ignore break codes for now
    }
    switch (scancode_state) {
    case SCANCODE_NORMAL:
        if (isprint(scancode_map[scancode])) {
            vga_putc(scancode_map[scancode]);
        } else if (scancode_map[scancode] == '\n') {
            vga_newline();
        } else if (scancode == 0x0E) { // Backspace
            vga_backspace();
        }
        break;

    case SCANCODE_EXTENDED_E0:
        // Handle extended scancodes if needed
        scancode_state = SCANCODE_NORMAL;
        break;
    default:
        scancode_state = SCANCODE_NORMAL;
        break;
    }
}
