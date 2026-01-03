#pragma once

#include "cpu_utils/cpu_utils.h"
#include "io/vga.h"

#define KEYBOARD_CTRL_DATA 0x60

typedef enum {
    SCANCODE_NORMAL,
    SCANCODE_EXTENDED_E0
} scancode_state_t;

void keyboard_handler_c(void);