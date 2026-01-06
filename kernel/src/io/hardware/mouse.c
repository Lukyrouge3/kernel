#include "io/hardware/mouse.h"
#include "io/hardware/hardware.h"
#include "io/io_utils.h"
#include "panic.h"

#include "io/printf/printf.h"

static uint8_t vga_col = 40; // Start center
static uint8_t vga_row = 12;

#define VGA_COLS 80
#define VGA_ROWS 25
#define CHAR_WIDTH 8
#define CHAR_HEIGHT (480 / VGA_ROWS)

void mouse_handler(void) {
    static uint8_t mouse_cycle = 0;
    static int8_t mouse_packet[3];

    uint8_t status = inb(PS2_CMD_PORT);
    if (!(status & 0x20)) {
        return;
    }

    uint8_t mouse_data = inb(PS2_DATA_PORT);
    switch (mouse_cycle) {
    case 0:
        mouse_packet[0] = mouse_data;
        if (!(mouse_data & 0x08))
            break;
        mouse_cycle++;
        break;

    case 1:
        mouse_packet[1] = mouse_data;
        mouse_cycle++;
        break;

    case 2:
        mouse_packet[2] = mouse_data;
        mouse_cycle = 0;

        int8_t delta_x = mouse_packet[1];
        int8_t delta_y = mouse_packet[2];

        // Update VGA position
        int16_t new_col = vga_col + (delta_x / CHAR_WIDTH);
        int16_t new_row = vga_row - (delta_y / CHAR_HEIGHT);

        // Clamp to VGA bounds
        if (new_col < 0)
            new_col = 0;
        if (new_col >= VGA_COLS)
            new_col = VGA_COLS - 1;
        if (new_row < 0)
            new_row = 0;
        if (new_row >= VGA_ROWS)
            new_row = VGA_ROWS - 1;

        vga_col = new_col;
        vga_row = new_row;

        break;
    }
}

void mouse_init(void) {
    uint8_t status;

    ps2_write_cmd(DISABLE_MOUSE);

    ps2_write_cmd(READ_COMMAND_BYTE);
    status = ps2_read_data();
    status |= 0x02;
    status &= ~0x20;
    ps2_write_cmd(WRITE_COMMAND_BYTE);
    ps2_write_data(status);
    ps2_write_cmd(ENABLE_MOUSE);

    // Enable data reporting
    ps2_write_cmd(WRITE_TO_MOUSE);
    ps2_write_data(0xF4);
    if (ps2_read_data() != OK_UNDERSTOOD) {
        PANIC("Mouse did not acknowledge data reporting command\n");
    }
    serial_printf("Mouse initialized successfully.\n");
}
