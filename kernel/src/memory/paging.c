#include "io/printf/printf.h"
#include "memory.h"
#include "panic.h"
#include "stdlib.h"

static uint32_t page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(4096))) = {0};

void paging_init(void) {

    for (int i = 0; i < PAGE_DIRECTORY_SIZE; i++) {
        page_directory[i] = 0; // Not present
    }

    uint32_t start = 0x0;
    uint32_t end = start + 0x400000; // Map first 4MB for now

    uint32_t first_dir = start >> 22;
    uint32_t last_dir = (end - 1) >> 22;

    for (uint32_t dir_index = first_dir; dir_index <= last_dir; dir_index++) {
        uint32_t *page = pmm_alloc_block();
        ASSERT(page != NULL); // Ensure we got a block, since it's critical we panic if it fails
        memset(page, 0, PAGE_SIZE);

        for (int page_index = 0; page_index < PAGE_TABLE_SIZE; page_index++) {
            uint32_t addr = (dir_index * PAGE_TABLE_SIZE + page_index) * PAGE_SIZE;

            if (addr >= start && addr < end) {
                page[page_index] =
                    (addr & PG_ADDR_MASK) | PG_PRESENT | PG_RW; // Present, kernel, read/write
            } else {
                page[page_index] = 0; // Not present
            }
        }
        serial_printf("Paging: Created page table for directory index %d at 0x%X\n", dir_index,
                      (uint32_t)page);
        page_directory[dir_index] =
            ((uint32_t)page & PG_ADDR_MASK) | PG_PRESENT | PG_RW; // Present, kernel, read/write
    }

    pmm_deinit_region(start, end - start); // Mark the mapped region as used

    serial_printf("Paging: Loading page directory at 0x%X, mapping from 0x%X to 0x%X\n",
                  (uint32_t)page_directory, start, end);

    load_page_directory(page_directory);
    enable_paging();
}

static uint32_t _addr_from_indexes(uint32_t dir, uint32_t page) {
    return (dir * PAGE_TABLE_SIZE + page) * PAGE_SIZE;
}

static uint32_t _paging_get_first_free(void) {
    for (uint32_t dir_index = 0; dir_index < PAGE_DIRECTORY_SIZE; dir_index++) {
        if ((page_directory[dir_index] & PG_PRESENT) == 0) {
            return _addr_from_indexes(dir_index, 0);
        }
        uint32_t *page_table = (uint32_t *)(page_directory[dir_index] & PG_ADDR_MASK);
        for (int page_index = 0; page_index < PAGE_TABLE_SIZE; page_index++) {
            if ((page_table[page_index] & PG_PRESENT) == 0) {
                return _addr_from_indexes(dir_index, page_index);
            }
        }
    }
    return 0;
}

void *paging_alloc(uint32_t size, bool user_accessible) {
    uint32_t pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    uint32_t allocated_memory = 0;

    serial_printf("Paging Alloc: Requesting %d bytes (%d pages), user_accessible=%d\n", size,
                  pages_needed, user_accessible);

    allocated_memory = _paging_get_first_frees(size);
    if (allocated_memory == 0) {
        return NULL; // No suitable space found
    }
    serial_printf("Paging Alloc: Found space at 0x%X\n", allocated_memory);

    for (uint32_t i = 0; i < pages_needed; i++) {
        uint32_t addr = allocated_memory + i * PAGE_SIZE;
        uint32_t dir_index = addr >> 22;
        uint32_t page_index = (addr >> 12) & 0x3FF;

        uint32_t pde = page_directory[dir_index];
        uint32_t *page = (uint32_t *)(page_directory[dir_index] & PG_ADDR_MASK);
        if ((pde & PG_PRESENT) == 0) {
            page = pmm_alloc_block();
            ASSERT(page != NULL); // Ensure we got a block, since it's critical we panic if it fails
            memset(page, 0, PAGE_SIZE);
            page_directory[dir_index] =
                ((uint32_t)page & PG_ADDR_MASK) | PG_PRESENT | PG_RW |
                (user_accessible ? PG_USER : 0); // Present, read/write, user/kernel
            serial_printf("Paging Alloc: Created new page table for dir %d at 0x%X\n", dir_index,
                          (uint32_t)page);
        } else if (user_accessible && !(pde & PG_USER)) {
            // Upgrade existing page table to user accessible
            page_directory[dir_index] |= PG_USER;
        }

        page[page_index] =
            (addr & PG_ADDR_MASK) | PG_PRESENT | PG_RW; // Present, kernel, read/write
        // serial_printf("Paging Alloc: Mapped page dir %d, page %d to addr 0x%X\n", dir_index,
        //               page_index, addr);
    }

    return (void *)allocated_memory;
}