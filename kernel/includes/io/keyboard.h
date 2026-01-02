#pragma once

#include "cpu_utils/cpu_utils.h"
#include "io/vga.h"

#define KEYBOARD_CTRL_DATA 0x60

static const char scancode_map[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', // Backspace
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',      // Enter
    0, // Ctrl
    'a','s','d','f','g','h','j','k','l',';','\'','`',0, '\\',
    'z','x','c','v','b','n','m',',','.','/',0, '*',0, ' ',          // Space
    // rest left 0
};


void __attribute__((naked)) irq1_handler(void);
