#pragma once

#include "io/keyboard.h"
#include <stdint.h>

#define IDT_FLAG_GATE_TASK 0x5
#define IDT_FLAG_GATE_16BIT_INT 0x6
#define IDT_FLAG_GATE_16BIT_TRAP 0x7
#define IDT_FLAG_GATE_32BIT_INT 0xE
#define IDT_FLAG_GATE_32BIT_TRAP 0xF

#define IDT_FLAG_RING0 0x00
#define IDT_FLAG_RING3 0x60
#define IDT_FLAG_PRESENT 0x80

struct idt_entry {
    uint16_t offset_low;  // bits 0–15 of handler
    uint16_t selector;    // GDT code segment
    uint8_t zero;         // must be 0
    uint8_t type_attr;    // type, privilege, present
    uint16_t offset_high; // bits 16–31 of handler
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

void idt_init();
void idt_handler(struct registers regs);

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void irq1(void);