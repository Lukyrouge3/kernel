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
int pmm_get_block_count(void);
void *pmm_alloc_block(void);
void *pmm_alloc_blocks(uint32_t size);
void pmm_free_block(void *block);
void pmm_free_blocks(void *block, uint32_t size);
int pmm_get_used_blocks(void);
uint32_t pmm_get_physical_memory_base(void);
uint32_t pmm_get_memory_size(void);

void mmap_set(int bit);
void mmap_unset(int bit);
bool mmap_test(int bit);
int32_t mmap_first_free(void);
int32_t mmap_first_free_sized(uint32_t size);

// === PAGING ===

// See: https://wiki.osdev.org/Paging#Page_Directory

#define PG_PRESENT (1u << 0)
#define PG_RW (1u << 1)
#define PG_USER (1u << 2)
#define PG_PWT (1u << 3)
#define PG_PCD (1u << 4)
#define PG_ACCESSED (1u << 5)
#define PG_DIRTY (1u << 6)
#define PG_PS (1u << 7)
#define PG_GLOBAL (1u << 8)
#define PG_AVL1 (1u << 9)
#define PG_AVL2 (1u << 10)
#define PG_AVL3 (1u << 11)
#define PG_ADDR_MASK 0xFFFFF000

#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE 1024
#define PAGE_SIZE 0x1000 // 4KB

// Defined in kernel/asm/paging.asm
extern void load_page_directory(uint32_t *table);
// Defined in kernel/asm/paging.asm
extern void enable_paging();

void paging_init(void);
void *paging_alloc(uint32_t size, bool user_accessible);
void paging_free(uint32_t addr, uint32_t size);

void *kmalloc(uint32_t size);
void kfree(void *ptr);