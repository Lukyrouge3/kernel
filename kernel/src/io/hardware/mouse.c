#include "io/hardware/mouse.h"
#include "io/io_utils.h"

#include "io/printf/printf.h"

void mouse_handler(void) {
    // Read the mouse data from port 0x60
    uint8_t mouse_data = inb(0x60);

    // For demonstration, we will just print the mouse data
    serial_printf("Mouse Data: 0x%02X\n", mouse_data);
}