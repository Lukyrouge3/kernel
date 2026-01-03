#include "cpu_utils/idt.h"
#include "io/keyboard.h"
#include "io/printf/printf.h"
#include "panic.h"

static struct idt_entry idt[256];
static struct idt_ptr idtp;

static void idt_set_gate(int n, uint32_t handler, uint8_t type_attr) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; // what segment to use in our gdt
    idt[n].zero = 0;
    idt[n].type_attr =
        handler ? (type_attr | IDT_FLAG_PRESENT) : 0; // Only set present if handler exists
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_init() { // TODO add more handlers for cpu interrupts
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;

    // Clear IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0); // default: no handler
    }

    idt_set_gate(0x21, (uint32_t)irq1_handler,
                 IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0); // keyboard IRQ1

    idt_set_gate(0, (uint32_t)isr0, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(1, (uint32_t)isr1, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(2, (uint32_t)isr2, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(3, (uint32_t)isr3, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(4, (uint32_t)isr4, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(5, (uint32_t)isr5, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(6, (uint32_t)isr6, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(7, (uint32_t)isr7, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(8, (uint32_t)isr8, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(9, (uint32_t)isr9, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(10, (uint32_t)isr10, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(11, (uint32_t)isr11, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(12, (uint32_t)isr12, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(13, (uint32_t)isr13, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(14, (uint32_t)isr14, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(15, (uint32_t)isr15, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(16, (uint32_t)isr16, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(17, (uint32_t)isr17, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(18, (uint32_t)isr18, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(19, (uint32_t)isr19, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(20, (uint32_t)isr20, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(21, (uint32_t)isr21, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(22, (uint32_t)isr22, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(23, (uint32_t)isr23, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(24, (uint32_t)isr24, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(25, (uint32_t)isr25, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(26, (uint32_t)isr26, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(27, (uint32_t)isr27, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(28, (uint32_t)isr28, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(29, (uint32_t)isr29, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(30, (uint32_t)isr30, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);
    idt_set_gate(31, (uint32_t)isr31, IDT_FLAG_GATE_32BIT_INT | IDT_FLAG_RING0);

    // Load IDT into CPU
    __asm__ volatile("lidt (%0)" : : "r"(&idtp));
    __asm__ volatile("sti"); // Enable interrupts
}

void idt_handler(struct registers regs) {
    serial_printf("Received interrupt: %d\n", regs.int_no);
    // Vous pouvez ajouter votre gestion d'interruptions ici
    // Par exemple, afficher un message d'erreur
    if (regs.int_no < 32) {
        PANIC("CPU Exception: Interrupt Number %d, err_code %d", regs.int_no, regs.err_code);
    }

    if (regs.int_no == 0x21) {
        keyboard_handler_c();
    }
}