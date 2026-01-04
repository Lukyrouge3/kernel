#pragma once
#include <stdint.h>

#ifndef KERNEL_SECTORS
#define KERNEL_SECTORS 20
#endif

struct E820Entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    // No acpi_extended - most BIOSes don't provide it
} __attribute__((packed));

extern uint16_t entry_count;
extern struct E820Entry *memory_map;

void check_memory_map(void);

#define PMM_BITMAP_BLOCK_SIZE 0x1000 // 4KB blocks

//! size of physical memory
extern uint32_t _memory_size;

//! number of blocks currently in use
extern uint32_t _used_blocks;

//! maximum number of available memory blocks
extern uint32_t _max_blocks;

//! memory map bit array. Each bit represents a memory block
extern uint32_t *_memory_map;

void pmm_init(uint32_t bitmap_location);