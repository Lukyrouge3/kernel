#include "io/printf/printf.h"
#include "memory.h"
#include "stdlib.h"

static uint32_t page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(4096))) = {0};

void paging_init(void) {

    for (int i = 0; i < PAGE_DIRECTORY_SIZE; i++) {
        page_directory[i] = PG_RW; // Not present, kernel, read/write
    }

    uint32_t start = 0x0;
    uint32_t end = start + 0x400000; // Map first 4MB for now

    uint32_t first_dir = start >> 22;
    uint32_t last_dir = (end - 1) >> 22;

    for (uint32_t dir_index = first_dir; dir_index <= last_dir; dir_index++) {
        uint32_t *page = pmm_alloc_block();
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

    serial_printf("Paging: Loading page directory at 0x%X, mapping from 0x%X to 0x%X\n",
                  (uint32_t)page_directory, start, end);

    load_page_directory(page_directory);
    enable_paging();
}