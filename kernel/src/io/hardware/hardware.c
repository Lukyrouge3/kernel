#include "io/hardware/hardware.h"
#include "io/io_utils.h"
#include "io/hardware/keyboard.h"
#include "io/hardware/mouse.h"

static void ps2_flush(void) {
    // Just flush any stale data from the buffer
    while (inb(PS2_CMD_PORT) & 0x01) {
        inb(PS2_DATA_PORT);
    }
}

void ps2_write_cmd(uint8_t cmd) {
    // Wait until input buffer is empty (bit 1 = 0)
    while (inb(PS2_CMD_PORT) & 0x02);
    outb(PS2_CMD_PORT, cmd);
}

uint8_t ps2_read_data(void) {
    // Wait until output buffer is full (bit 0 = 1)
    while (!(inb(PS2_CMD_PORT) & 0x01));
    return inb(PS2_DATA_PORT);
}

void ps2_write_data(uint8_t data) {
    // Wait until input buffer is empty
    while (inb(PS2_CMD_PORT) & 0x02);
    outb(PS2_DATA_PORT, data);
}

void init_hardware_io(void) {
    ps2_flush();
    keyboard_init();
    mouse_init();
}