#include "cpu_utils/cpu_utils.h"

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