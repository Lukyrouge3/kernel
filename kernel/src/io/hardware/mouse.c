#include "io/hardware/mouse.h"
#include "io/hardware/hardware.h"
#include "io/io_utils.h"
#include "panic.h"

#include "io/printf/printf.h"

void mouse_handler(void) {
    static uint8_t mouse_cycle = 0;
    static int8_t mouse_packet[3];
    
        uint8_t status = ps2_read_data();
    serial_printf("Mouse IRQ - Status: 0x%X (bit5=%d, bit0=%d)\n", 
                 status, !!(status & 0x20), !!(status & 0x01));
    
    if (!(status & 0x01)) {
        serial_printf("  -> No data in buffer!\n");
        return;
    }
    
    if (!(status & 0x20)) {
        serial_printf("  -> Data is from KEYBOARD not mouse!\n");
        return;
    }
    
    uint8_t mouse_data = ps2_read_data();
    
    switch (mouse_cycle) {
        case 0:
            mouse_packet[0] = mouse_data;
            if (!(mouse_data & 0x08)) break;  // Invalid, stay at 0
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
            
            serial_printf("Mouse moved: X=%d, Y=%d\n", delta_x, delta_y);
            break;
    }
}

void mouse_init(void) {
    uint8_t status, response;

    ps2_write_cmd(DISABLE_MOUSE);
    ps2_write_cmd(READ_COMMAND_BYTE);
    status = ps2_read_data();
    status |= 0x02;
    status &= ~0x20;
    ps2_write_cmd(WRITE_COMMAND_BYTE);
    ps2_write_data(status);
    ps2_write_cmd(ENABLE_MOUSE);
    
    serial_printf("=== Mouse Init Debug ===\n");
    
    // Reset
    ps2_write_cmd(WRITE_TO_MOUSE);
    ps2_write_data(0xFF);
    response = ps2_read_data();
    serial_printf("Reset ACK: 0x%X (expect 0xFA)\n", response);
    response = ps2_read_data();
    serial_printf("Self-test: 0x%X (expect 0xAA)\n", response);
    response = ps2_read_data();
    serial_printf("Mouse ID: 0x%X (expect 0x00)\n", response);
    
    // Check current mode
    ps2_write_cmd(WRITE_TO_MOUSE);
    ps2_write_data(0xEB);  // Read data (gives one packet)
    response = ps2_read_data();
    serial_printf("Read data ACK: 0x%X\n", response);
    // This should give us 3 bytes
    uint8_t b1 = ps2_read_data();
    uint8_t b2 = ps2_read_data();
    uint8_t b3 = ps2_read_data();
    serial_printf("Status packet: 0x%X 0x%X 0x%X\n", b1, b2, b3);
    
    // Set stream mode
    ps2_write_cmd(WRITE_TO_MOUSE);
    ps2_write_data(0xEA);
    response = ps2_read_data();
    serial_printf("Stream mode ACK: 0x%X (expect 0xFA)\n", response);
    
    // Set sample rate to 200 (higher rate)
    ps2_write_cmd(WRITE_TO_MOUSE);
    ps2_write_data(0xF3);
    response = ps2_read_data();
    serial_printf("Sample rate cmd ACK: 0x%X\n", response);
    
    ps2_write_cmd(WRITE_TO_MOUSE);
    ps2_write_data(200);  // Try higher rate
    response = ps2_read_data();
    serial_printf("Sample rate value ACK: 0x%X\n", response);
    
    // Enable data reporting
    ps2_write_cmd(WRITE_TO_MOUSE);
    ps2_write_data(0xF4);
    response = ps2_read_data();
    serial_printf("Enable reporting ACK: 0x%X (expect 0xFA)\n", response);
    
    serial_printf("=== Mouse Init Complete ===\n");
}
