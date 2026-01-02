#pragma once

#include <stdint.h>
#include "io/keyboard.h"

struct idt_entry {
    uint16_t offset_low;  // bits 0–15 of handler
    uint16_t selector;    // GDT code segment
    uint8_t zero;         // must be 0
    uint8_t type_attr;    // type, privilege, present
    uint16_t offset_high; // bits 16–31 of handler
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idtp;

void idt_init();