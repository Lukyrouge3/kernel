#include "cpu_utils/cpu_utils.h"
#include "io/printf/printf.h"

uint16_t read_cs(void) {
    uint16_t cs;
    __asm__ volatile("mov %%cs, %0" : "=r"(cs));
    return cs;
}

uint16_t read_ds(void) {
    uint16_t ds;
    __asm__ volatile("mov %%ds, %0" : "=r"(ds));
    return ds;
}

uint16_t read_ss(void) {
    uint16_t ss;
    __asm__ volatile("mov %%ss, %0" : "=r"(ss));
    return ss;
}

void get_cpu_registers(cpu_registers_t *regs) {
    __asm__ volatile("mov %%eax, %0\n"
                     "mov %%ebx, %1\n"
                     "mov %%ecx, %2\n"
                     "mov %%edx, %3\n"
                     "mov %%esi, %4\n"
                     "mov %%edi, %5\n"
                     "mov %%ebp, %6\n"
                     "mov %%esp, %7\n"
                     "call 1f\n"
                     "1: pop %%eax\n"
                     "mov %%eax, %8\n"
                     "pushf\n"
                     "pop %%eax\n"
                     "mov %%eax, %9\n"
                     : "=m"(regs->eax), "=m"(regs->ebx), "=m"(regs->ecx), "=m"(regs->edx),
                       "=m"(regs->esi), "=m"(regs->edi), "=m"(regs->ebp), "=m"(regs->esp),
                       "=m"(regs->eip), "=m"(regs->eflags)
                     :
                     : "eax");
}

void print_cpu_registers(const cpu_registers_t *regs) {
    serial_printf("EAX: 0x%X | EBX: 0x%X | ECX: 0x%X | EDX: 0x%X\n", regs->eax, regs->ebx,
                  regs->ecx, regs->edx);
    serial_printf("ESI: 0x%X | EDI: 0x%X | EBP: 0x%X | ESP: 0x%X\n", regs->esi, regs->edi,
                  regs->ebp, regs->esp);
    serial_printf("EIP: 0x%X | EFLAGS: 0x%X\n", regs->eip, regs->eflags);

void save_cpu_state() {
    __asm__ volatile(
        "push %%eax\n"
        "push %%ecx\n"
        "push %%edx\n"
        "push %%ebx\n"
        "push %%esp\n"
        "push %%ebp\n"
        "push %%esi\n"
        "push %%edi\n"
        "push %%ds\n"  //push data segment
        "push %%es\n"  //push extra segment
        :
        :
        : "memory"
    );
}

void restore_cpu_state() {
    __asm__ volatile(
        "pop %%es\n"
        "pop %%ds\n"
        "pop %%edi\n"
        "pop %%esi\n"
        "pop %%ebp\n"
        "pop %%esp\n"
        "pop %%ebx\n"
        "pop %%edx\n"
        "pop %%ecx\n"
        "pop %%eax\n"
        :
        :
        : "memory"
    );
}