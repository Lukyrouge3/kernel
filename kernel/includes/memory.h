#pragma once
#include <stdbool.h>
#include <stddef.h>
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

void pmm_init(uint32_t bitmap_location);
void pmm_init_region(uint32_t base, size_t length);
void pmm_deinit_region(uint32_t base, size_t length);
int pmm_get_block_count();
void *pmm_alloc_block();
void *pmm_alloc_blocks(uint32_t size);
void pmm_free_block(void *block);
void pmm_free_blocks(void *block, uint32_t size);
int pmm_get_used_blocks();

void mmap_set(int bit);
void mmap_unset(int bit);
bool mmap_test(int bit);
uint32_t mmap_first_free();
uint32_t mmap_first_free_sized(uint32_t size);