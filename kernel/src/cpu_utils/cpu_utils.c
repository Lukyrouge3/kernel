#include "cpu_utils/cpu_utils.h"

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