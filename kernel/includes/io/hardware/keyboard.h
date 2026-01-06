#pragma once

#include "cpu_utils/cpu_utils.h"
#include "io/vga.h"

typedef enum {
    SCANCODE_NORMAL,
    SCANCODE_EXTENDED_E0
} scancode_state_t;

void keyboard_handler(void);
void keyboard_init(void);