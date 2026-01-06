#pragma once

#include <stdint.h>

#define PS2_DATA_PORT 0x60
#define PS2_CMD_PORT 0x64

#define READ_COMMAND_BYTE 0x20
#define WRITE_COMMAND_BYTE 0x60
#define ENABLE_MOUSE 0xA8
#define DISABLE_MOUSE 0xA7
#define ENABLE_KEYBOARD 0xAE
#define DISABLE_KEYBOARD 0xAD
#define WRITE_TO_MOUSE 0xD4
#define OK_UNDERSTOOD 0xFA

void ps2_write_cmd(uint8_t cmd);
uint8_t ps2_read_data(void);
void ps2_write_data(uint8_t data);
void init_hardware_io(void);