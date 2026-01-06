#include "io/hardware/keyboard.h"
#include "io/hardware/hardware.h"
#include "io/io_utils.h"
#include "panic.h"
#include "stdlib.h"
#include "io/printf/printf.h"

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

void keyboard_handler(void) {
    uint8_t scancode = ps2_read_data();
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
        } else if (scancode_map[scancode] == '\b') { // Backspace
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

// usually unnecessary but done for safety
void keyboard_init(void) {
    uint8_t status;

    ps2_write_cmd(DISABLE_KEYBOARD);

    // Get current command byte
    ps2_write_cmd(READ_COMMAND_BYTE);
    status = ps2_read_data();

    // Enable keyboard interrupts (bit 0) and disable keyboard clock disable (bit 4)
    status |= 0x01;  // Enable IRQ1
    status &= ~0x10; // Enable keyboard clock

    // Write modified command byte back
    ps2_write_cmd(WRITE_COMMAND_BYTE);
    ps2_write_data(status);

    ps2_write_cmd(ENABLE_KEYBOARD);

    // Tell keyboard to start sending scancodes
    ps2_write_data(0xF4); // Enable scanning
    if (ps2_read_data() != OK_UNDERSTOOD) {
        PANIC("Keyboard did not acknowledge data reporting command\n"); // Read ACK (0xFA)
    }
    serial_printf("Keyboard initialized successfully.\n");
}