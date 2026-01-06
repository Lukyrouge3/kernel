#include "memory.h"
#include "io/printf/printf.h"
#include "panic.h"
#include "stdlib.h"
#include <stdint.h>

static uint32_t _pmm_memory_size = 0;
static uint32_t _pmm_used_blocks = 0;
static uint32_t _pmm_max_blocks = 0;
static uint32_t *_pmm_memory_map = 0;
static uint32_t _pmm_physical_memory_base = 0;

void check_memory_map(void) {
    uint16_t entry_count = *(uint16_t *)0x7FFE;
    struct E820Entry *memory_map = (struct E820Entry *)0x8000;

    entry_count = *(uint16_t *)0x7FFE;
    memory_map = (struct E820Entry *)0x8000;

    // Validate entry count is sane
    ASSERT(entry_count > 0);
    ASSERT(entry_count <= 128); // E820 typically has < 20 entries, 128 is a safe max

    serial_printf("E820 Memory Map: %d entries\n", entry_count);

    // Now try reading the entries
    for (int i = 0; i < entry_count; i++) {
        // Validate entry type is valid (1-5 are standard E820 types)
        ASSERT(memory_map[i].type >= 1 && memory_map[i].type <= 5);
        // Validate length is non-zero
        ASSERT(memory_map[i].length > 0);

        uint32_t base_hi = (uint32_t)(memory_map[i].base >> 32);
        uint32_t base_lo = (uint32_t)(memory_map[i].base & 0xFFFFFFFF);
        uint32_t len_hi = (uint32_t)(memory_map[i].length >> 32);
        uint32_t len_lo = (uint32_t)(memory_map[i].length & 0xFFFFFFFF);

        if (base_hi) {
            serial_printf("Entry %d: Base=0x%x%x, ", i, base_hi, base_lo);
        } else {
            serial_printf("Entry %d: Base=0x%x, ", i, base_lo);
        }

        if (len_hi) {
            serial_printf("Length=0x%x%x, Type=%d\n", len_hi, len_lo, memory_map[i].type);
        } else {
            serial_printf("Length=0x%x, Type=%d\n", len_lo, memory_map[i].type);
        }
    }
}

void pmm_init(uint32_t bitmap_location) {
    struct E820Entry *memory_map = (struct E820Entry *)0x8000;

    _pmm_memory_size =
        memory_map[3]
            .length; // For now we only take the 4th entry cause we dont have a way to map others
    _pmm_max_blocks = _pmm_memory_size / PMM_BITMAP_BLOCK_SIZE;
    _pmm_memory_map = (uint32_t *)bitmap_location;
    _pmm_physical_memory_base = memory_map[3].base;

    serial_printf("Location of PMM bitmap: 0x%x\n", bitmap_location);

    // By default we mark all memory as used
    memset(_pmm_memory_map, 0xff,
           pmm_get_block_count() / 8); // We divide by 8 because memset works in bytes
    _pmm_used_blocks = pmm_get_block_count();

    serial_printf("PMM initialized: %d KB total, %d blocks\n", _pmm_memory_size / 1024,
                  _pmm_max_blocks);

    pmm_init_region(_pmm_physical_memory_base, _pmm_memory_size);
}

void mmap_set(int bit) {
    int bitmap_index = bit / 32;
    int block_bit = bit % 32;

    _pmm_memory_map[bitmap_index] |= (1 << block_bit);
}

void mmap_unset(int bit) {
    int bitmap_index = bit / 32;
    int block_bit = bit % 32;

    _pmm_memory_map[bitmap_index] &= ~(1 << block_bit);
}

bool mmap_test(int bit) {
    int bitmap_index = bit / 32;
    int block_bit = bit % 32;

    return (_pmm_memory_map[bitmap_index] & (1 << block_bit)) != 0;
}

int pmm_get_block_count() {
    return _pmm_max_blocks;
}

void pmm_init_region(uint32_t base, size_t length) {
    uint32_t start_block = (base - _pmm_physical_memory_base) / PMM_BITMAP_BLOCK_SIZE;
    uint32_t block_count = (length + PMM_BITMAP_BLOCK_SIZE - 1) / PMM_BITMAP_BLOCK_SIZE;

    for (uint32_t i = 0; i < block_count; i++) {
        mmap_unset(start_block + i);
        _pmm_used_blocks--;
    }

    // We want to keep 0 block reserved
    if (!mmap_test(0)) {
        mmap_set(0);
        _pmm_used_blocks++;
    }
}

// For now we use a simple first-fit algorithm but it sucks, we can improve it later
uint32_t mmap_first_free() {
    for (uint32_t i = 0; i < pmm_get_block_count() / 32; i++) {
        if (_pmm_memory_map[i] != 0xffffffff) { // Not all blocks used
            for (int j = 0; j < 32; j++) {
                int bit = 1 << j;

                if (!(_pmm_memory_map[i] & bit)) {
                    return i * 32 + j;
                }
            }
        }
    }
    return -1; // No free blocks
}

uint32_t mmap_first_free_sized(uint32_t size) {
    uint32_t free_count = 0;
    uint32_t start_bit = 0;

    for (uint32_t i = 0; i < pmm_get_block_count(); i++) {
        if (!mmap_test(i)) {
            if (free_count == 0) {
                start_bit = i;
            }
            free_count++;

            if (free_count >= size) {
                return start_bit;
            }
        } else {
            free_count = 0;
        }
    }
    return -1; // No suitable block found
}

void *pmm_alloc_block() {
    uint32_t free_bit = mmap_first_free();
    if (free_bit == (uint32_t)-1) {
        return NULL; // No free blocks
    }

    mmap_set(free_bit);

    uint32_t adress =
        free_bit * PMM_BITMAP_BLOCK_SIZE +
        _pmm_physical_memory_base; // Change to a better system to map different physical spaces

    _pmm_used_blocks++;

    return (void *)adress;
}

void *pmm_alloc_blocks(uint32_t size) {
    uint32_t free_bit = mmap_first_free_sized(size);
    if (free_bit == (uint32_t)-1) {
        return NULL; // No free blocks
    }

    for (uint32_t i = 0; i < size; i++) {
        mmap_set(free_bit + i);
    }

    uint32_t adress =
        free_bit * PMM_BITMAP_BLOCK_SIZE +
        _pmm_physical_memory_base; // Change to a better system to map different physical spaces

    _pmm_used_blocks += size;

    return (void *)adress;
}

void pmm_free_block(void *block) {
    uint32_t addr = (uint32_t)block;
    uint32_t block_num = (addr - _pmm_physical_memory_base) / PMM_BITMAP_BLOCK_SIZE;

    mmap_unset(block_num);
    _pmm_used_blocks--;
}

void pmm_free_blocks(void *block, uint32_t size) {
    uint32_t addr = (uint32_t)block;
    uint32_t block_num = (addr - _pmm_physical_memory_base) / PMM_BITMAP_BLOCK_SIZE;

    for (uint32_t i = 0; i < size; i++) {
        mmap_unset(block_num + i);
    }
    _pmm_used_blocks -= size;
}

int pmm_get_used_blocks() {
    return _pmm_used_blocks;
}

void pmm_deinit_region(uint32_t base, size_t length) {
    uint32_t start_block = (base - _pmm_physical_memory_base) / PMM_BITMAP_BLOCK_SIZE;
    uint32_t block_count = (length + PMM_BITMAP_BLOCK_SIZE - 1) / PMM_BITMAP_BLOCK_SIZE;

    for (uint32_t i = 0; i < block_count; i++) {
        mmap_set(start_block + i);
        _pmm_used_blocks++;
    }
}