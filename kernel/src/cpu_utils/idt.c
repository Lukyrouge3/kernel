#include "cpu_utils/idt.h"

static struct idt_entry idt[256];
static struct idt_ptr idtp;

static void set_idt_gate(int n, uint32_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; //what segment to use in our gdt
    idt[n].zero = 0;
    idt[n].type_attr = 0b10001110;//see idt type attr mask
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_init() {//TODO add more handlers for cpu interupts
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;

    // Clear IDT
    for (int i = 0; i < 256; i++) {
        set_idt_gate(i, 0); // default: no handler
    }

    set_idt_gate(0x21, (uint32_t)irq1_handler);

    // Load IDT into CPU
    __asm__ volatile("lidt (%0)" : : "r"(&idtp));
    __asm__ volatile("sti"); // Enable interrupts
}