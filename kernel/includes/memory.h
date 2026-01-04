#pragma once
#include <stdint.h>

struct E820Entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    // No acpi_extended - most BIOSes don't provide it
} __attribute__((packed));

extern uint16_t entry_count;
extern struct E820Entry *memory_map;

void check_memory_map(void);