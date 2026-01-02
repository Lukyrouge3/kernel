#pragma once

#include "cpu_utils/cpu_utils.h"
#include "io/vga.h"

#define KEYBOARD_CTRL_DATA 0x60

void __attribute__((naked)) irq1_handler(void);