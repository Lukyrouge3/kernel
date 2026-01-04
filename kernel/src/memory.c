#include "memory.h"
#include "io/printf/printf.h"
#include "panic.h"
#include <stdint.h>

uint32_t _memory_size = 0;
uint32_t _used_blocks = 0;
uint32_t _max_blocks = 0;
uint32_t *_memory_map = 0;

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

    _memory_size = memory_map[3].length;
    _max_blocks = _memory_size / PMM_BITMAP_BLOCK_SIZE;
    _memory_map = (uint32_t *)bitmap_location;

    serial_printf("Location of PMM bitmap: 0x%x\n", bitmap_location);

    // // Clear the memory map
    for (uint32_t i = 0; i < (_max_blocks / 32) + 1; i++) {
        _memory_map[i] = 0xf;
        // serial_printf("PMM bitmap[0x%x] (0x%x) = 0x%x\n", i, &_memory_map[i], _memory_map[i]);
    }

    serial_printf("PMM initialized: %d KB total, %d blocks\n", _memory_size / 1024, _max_blocks);
}