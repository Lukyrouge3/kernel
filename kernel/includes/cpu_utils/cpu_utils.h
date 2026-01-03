#pragma once

#include <stdint.h>

uint16_t read_cs(void);
uint16_t read_ds(void);
uint16_t read_ss(void);

typedef struct {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t eip;
    uint32_t eflags;
} cpu_registers_t;

void get_cpu_registers(cpu_registers_t *regs);
void print_cpu_registers(const cpu_registers_t *regs);
void save_cpu_state();
void restore_cpu_state();

void halt_cpu();

uint32_t read_eflags(void);
void write_eflags(uint32_t flags);
void disable_interrupts(void);
void enable_interrupts(void);