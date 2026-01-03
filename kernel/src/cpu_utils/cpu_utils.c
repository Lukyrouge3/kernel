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
    // Save registers without modifying them.
    // We must be careful: EAX and EBX will be used as scratch registers,
    // so we save them to the stack first, then use them.
    __asm__ volatile("push %%eax\n"       // Save original EAX on stack
                     "push %%ebx\n"       // Save original EBX on stack
                     "mov %%ecx, %2\n"    // Save ECX
                     "mov %%edx, %3\n"    // Save EDX
                     "mov %%esi, %4\n"    // Save ESI
                     "mov %%edi, %5\n"    // Save EDI
                     "mov %%ebp, %6\n"    // Save EBP
                     "mov %%esp, %%eax\n" // Get current ESP
                     "add $8, %%eax\n"    // Adjust for the two pushes above
                     "mov %%eax, %7\n"    // Save adjusted ESP
                     "call 1f\n"          // Push return address (EIP) onto stack
                     "1: pop %%eax\n"     // Pop EIP into EAX
                     "mov %%eax, %8\n"    // Save EIP
                     "pushf\n"            // Push EFLAGS onto stack
                     "pop %%eax\n"        // Pop EFLAGS into EAX
                     "mov %%eax, %9\n"    // Save EFLAGS
                     "pop %%ebx\n"        // Restore EBX (LIFO: pushed second, popped first)
                     "mov %%ebx, %1\n"    // Save original EBX value
                     "pop %%eax\n"        // Restore EAX (LIFO: pushed first, popped second)
                     "mov %%eax, %0\n"    // Save original EAX value
                     : "=m"(regs->eax), "=m"(regs->ebx), "=m"(regs->ecx), "=m"(regs->edx),
                       "=m"(regs->esi), "=m"(regs->edi), "=m"(regs->ebp), "=m"(regs->esp),
                       "=m"(regs->eip), "=m"(regs->eflags)
                     :
                     : "eax", "ebx", "memory");
}

void print_cpu_registers(const cpu_registers_t *regs) {
    serial_printf("EAX: 0x%X | EBX: 0x%X | ECX: 0x%X | EDX: 0x%X\n", regs->eax, regs->ebx,
                  regs->ecx, regs->edx);
    serial_printf("ESI: 0x%X | EDI: 0x%X | EBP: 0x%X | ESP: 0x%X\n", regs->esi, regs->edi,
                  regs->ebp, regs->esp);
    serial_printf("EIP: 0x%X | EFLAGS: 0x%X\n", regs->eip, regs->eflags);
}

void save_cpu_state() {
    __asm__ volatile("push %%eax\n"
                     "push %%ecx\n"
                     "push %%edx\n"
                     "push %%ebx\n"
                     "push %%esp\n"
                     "push %%ebp\n"
                     "push %%esi\n"
                     "push %%edi\n"
                     "push %%ds\n" // push data segment
                     "push %%es\n" // push extra segment
                     :
                     :
                     : "memory");
}

void restore_cpu_state() {
    __asm__ volatile("pop %%es\n"
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
                     : "memory");
}

void halt_cpu() {
    __asm__ volatile("hlt");
}

uint32_t read_eflags(void) {
    uint32_t flags;
    __asm__ volatile("pushfl; popl %0" : "=r"(flags));
    return flags;
}

void write_eflags(uint32_t flags) {
    __asm__ volatile("pushl %0; popfl" : : "r"(flags) : "cc");
}

void disable_interrupts(void) {
    __asm__ volatile("cli");
}

void enable_interrupts(void) {
    __asm__ volatile("sti");
}