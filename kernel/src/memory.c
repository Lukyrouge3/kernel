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

static bool _pmm_check_address(uint32_t addr) {
    return addr >= _pmm_physical_memory_base && addr < _pmm_physical_memory_base + _pmm_memory_size;
}

static bool _pmm_check_address_range(uint32_t addr, size_t length) {
    return _pmm_check_address(addr) && _pmm_check_address(addr + length - 1);
}

static void *_pmm_align_address(uint32_t addr) {
    return (void *)(addr & ~(PMM_BITMAP_BLOCK_SIZE - 1));
}

//! Validate and print the E820 memory map from BIOS
//! @note Reads from fixed BIOS locations (0x7FFE for count, 0x8000 for entries)
//! @note Asserts that all entries are valid (type 1-5, non-zero length)
void check_memory_map(void) {
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

//! Initialize the Physical Memory Manager
//! @param bitmap_location Physical address where the bitmap will be stored
//! @note Marks all memory as used by default, then marks the first entry as free
//! @note Currently only manages the 4th E820 entry (limitation for future improvement)
void pmm_init(uint32_t bitmap_location) {
    ASSERT(entry_count >= 4);
    ASSERT(memory_map[3].type == 1);
    ASSERT(memory_map[3].length <= UINT32_MAX); // To avoid 32-bit overflow in _pmm_memory_size
    ASSERT(memory_map[3].base <= UINT32_MAX);

    _pmm_memory_size =
        memory_map[3]
            .length; // For now we only take the 4th entry cause we dont have a way to map others
    _pmm_max_blocks = _pmm_memory_size / PMM_BITMAP_BLOCK_SIZE;
    _pmm_memory_map = (uint32_t *)bitmap_location;
    _pmm_physical_memory_base = memory_map[3].base;

    serial_printf("Location of PMM bitmap: 0x%x\n", bitmap_location);

    // By default we mark all memory as used
    memset(_pmm_memory_map, 0xff,
           (pmm_get_block_count() + 7) / 8); // Round up to ensure all bits have space

    _pmm_used_blocks = pmm_get_block_count();
    serial_printf("PMM initialized: %d KB total, %d blocks\n", _pmm_memory_size / 1024,
                  _pmm_max_blocks);

    pmm_init_region(_pmm_physical_memory_base, _pmm_memory_size);
}

//! Set a bit in the memory bitmap to mark a block as used
//! @param bit The block index to mark as used
//! Prints a warning if the bit is out of range
void mmap_set(int bit) {
    if (bit < 0 || bit >= (int)pmm_get_block_count()) {
        serial_printf("mmap_set: Attempt to set bit out of range %d!\n", bit);
        return;
    }

    int bitmap_index = bit / 32;
    int block_bit = bit % 32;

    _pmm_memory_map[bitmap_index] |= (1 << block_bit);
}

//! Clear a bit in the memory bitmap to mark a block as free
//! @param bit The block index to mark as free
//! Prints a warning if the bit is out of range
void mmap_unset(int bit) {
    if (bit < 0 || bit >= (int)pmm_get_block_count()) {
        serial_printf("mmap_unset: Attempt to unset bit out of range %d!\n", bit);
        return;
    }

    int bitmap_index = bit / 32;
    int block_bit = bit % 32;

    _pmm_memory_map[bitmap_index] &= ~(1 << block_bit);
}

//! Test if a bit in the memory bitmap is set (block is used)
//! @param bit The block index to test
//! @return true if the block is marked as used, false if free or out of range
bool mmap_test(int bit) {
    if (bit < 0 || bit >= (int)pmm_get_block_count()) {
        serial_printf("mmap_test: Attempt to test bit out of range %d!\n", bit);
        return false;
    }

    int bitmap_index = bit / 32;
    int block_bit = bit % 32;

    return (_pmm_memory_map[bitmap_index] & (1 << block_bit)) != 0;
}

int pmm_get_block_count() {
    return _pmm_max_blocks;
}

//! Mark a memory region as free and available for allocation
//! @param base Absolute physical address (must be block-aligned)
//! @param length Size of the region in bytes
//! @note Asserts that the region is within managed memory and properly aligned
void pmm_init_region(uint32_t base, size_t length) {
    ASSERT(_pmm_check_address_range(base, length));
    ASSERT((base % PMM_BITMAP_BLOCK_SIZE) == 0);

    uint32_t start_block = (base - _pmm_physical_memory_base) / PMM_BITMAP_BLOCK_SIZE;
    uint32_t block_count = (length + PMM_BITMAP_BLOCK_SIZE - 1) / PMM_BITMAP_BLOCK_SIZE;

    for (uint32_t i = 0; i < block_count; i++) {
        if (!mmap_test(start_block + i))
            continue;

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
// We use (pmm_get_block_count() + 31) / 32 to ensure we cover all bits in the bitmap
// (basically a round up)
int32_t mmap_first_free() {
    for (int i = 0; i < (pmm_get_block_count() + 31) / 32; i++) {
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


//! Find the first contiguous sequence of free blocks of a given size
//! @param size Number of contiguous blocks needed
//! @return Block index of the start of the free region, or -1 if not found
//! @note Uses a simple first-fit algorithm
int32_t mmap_first_free_sized(uint32_t size) {
    uint32_t free_count = 0;
    uint32_t start_bit = 0;

    for (int i = 0; i < pmm_get_block_count(); i++) {
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

//! Allocate a single 4KB memory block
//! @return Physical address of the allocated block, or NULL if no free blocks
void *pmm_alloc_block() {
    if (pmm_get_used_blocks() >= pmm_get_block_count()) { // No free blocks
        return NULL;
    }

    int32_t free_bit = mmap_first_free();
    if (free_bit == -1) {
        return NULL; // No free blocks
    }

    mmap_set(free_bit);

    uint32_t address =
        free_bit * PMM_BITMAP_BLOCK_SIZE +
        _pmm_physical_memory_base; // Change to a better system to map different physical spaces

    _pmm_used_blocks++;

    return (void *)address;
}

//! Allocate multiple contiguous 4KB memory blocks
//! @param size Number of blocks to allocate
//! @return Physical address of the first allocated block, or NULL if not enough contiguous free blocks
void *pmm_alloc_blocks(uint32_t size) {
    if (size == 0) {
        return NULL;
    }
    if (size > (uint32_t)(pmm_get_block_count() - pmm_get_used_blocks())) {
        // Not enough free blocks
        return NULL;
    }

    int32_t free_bit = mmap_first_free_sized(size);
    if (free_bit == -1) {
        return NULL; // No free blocks
    }

    for (uint32_t i = 0; i < size; i++) {
        mmap_set(free_bit + i);
    }

    uint32_t address =
        free_bit * PMM_BITMAP_BLOCK_SIZE +
        _pmm_physical_memory_base; // Change to a better system to map different physical spaces

    _pmm_used_blocks += size;

    return (void *)address;
}

//! Free a single allocated memory block
//! @param block Physical address of the block to free (will be aligned down to block boundary)
//! @note Prints a warning if the block is outside managed memory or already free
void pmm_free_block(void *block) {
    uint32_t addr = (uint32_t)_pmm_align_address((uint32_t)block);
    if (!_pmm_check_address(addr)) {
        serial_printf("PMM Free Block: Attempt to free block outside managed memory!\n");
        return;
    }

    uint32_t block_num = (addr - _pmm_physical_memory_base) / PMM_BITMAP_BLOCK_SIZE;

    if (!mmap_test(block_num)) {
        serial_printf("PMM Free Block: Attempt to free already free block!\n");
        return;
    }
    mmap_unset(block_num);
    _pmm_used_blocks--;
}

//! Free multiple contiguous allocated memory blocks
//! @param block Physical address of the first block to free (will be aligned down to block boundary)
//! @param size Number of blocks to free
//! @note Prints a warning if any block is outside managed memory or already free
void pmm_free_blocks(void *block, uint32_t size) {
    uint32_t addr = (uint32_t)_pmm_align_address((uint32_t)block);
    if (!_pmm_check_address_range(addr, size * PMM_BITMAP_BLOCK_SIZE)) {
        serial_printf("PMM Free Blocks: Attempt to free blocks outside managed memory!\n");
        return;
    }

    uint32_t block_num = (addr - _pmm_physical_memory_base) / PMM_BITMAP_BLOCK_SIZE;

    for (uint32_t i = 0; i < size; i++) {
        if (mmap_test(block_num + i)) {
            _pmm_used_blocks--;
            mmap_unset(block_num + i);
        } else {
            serial_printf("PMM Free Blocks: Attempt to free already free block %d!\n",
                          block_num + i);
        }
    }
}

int pmm_get_used_blocks() {
    return _pmm_used_blocks;
}

//! Mark a memory region as used and unavailable for allocation
//! @param base Absolute physical address (must be block-aligned)
//! @param length Size of the region in bytes
//! @note Asserts that the region is within managed memory and properly aligned
//! @note Inverse of pmm_init_region - marks blocks as used
void pmm_deinit_region(uint32_t base, size_t length) {
    // Since it's supposed to be a system call we can assert
    ASSERT(_pmm_check_address_range(base, length));
    ASSERT((base % PMM_BITMAP_BLOCK_SIZE) == 0); // We check if the base is aligned

    uint32_t start_block = (base - _pmm_physical_memory_base) / PMM_BITMAP_BLOCK_SIZE;
    uint32_t block_count = (length + PMM_BITMAP_BLOCK_SIZE - 1) / PMM_BITMAP_BLOCK_SIZE;

    for (uint32_t i = 0; i < block_count; i++) {
        if (mmap_test(start_block + i))
            continue;
        mmap_set(start_block + i);
        _pmm_used_blocks++;
    }
}