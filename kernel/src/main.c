#include "cpu_utils/cpu_utils.h"
#include "cpu_utils/idt.h"
#include "io/pic.h"
#include "io/printf/printf.h"
#include "io/serial.h"
#include "io/vga.h"
#include "panic.h"
#include <stdint.h>

void _start(void) __attribute__((section(".init")));

static void assert_protected_mode(void) {
    uint16_t cs = read_cs();
    ASSERT((cs & 0x3) == 0); // Ensure CPL is 0 (kernel mode)

    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    ASSERT(cr0 & 0x1); // Ensure PE bit is set (protected mode)
}

static void assert_flat_segments(void) {
    ASSERT(read_cs() == 0x08);
    ASSERT(read_ds() == 0x10);
    ASSERT(read_ss() == 0x10);
    /*
    In a flat memory model, all segment selectors should point to segments
    that cover the entire address space. Here we assume that the GDT is set up
    such that code segment is at 0x08 and data/stack segments are at 0x10.
    */
}

void _start(void) {
    serial_init_com1();
    assert_protected_mode();
    assert_flat_segments();
    pic_remap();
    idt_init();

    clear_screen();

    for (;;) {
    }
}