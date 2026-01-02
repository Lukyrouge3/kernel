#pragma once

__attribute__((noreturn)) void cpu_halt_forever(void);
__attribute__((noreturn)) void panic_impl(const char *file, int line, const char *func,
                                          const char *format, ...);

#define PANIC(format, ...) panic_impl(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)

#define ASSERT(cond)                                                                               \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            PANIC("Assertion failed: %s", #cond);                                                  \
        }                                                                                          \
    } while (0)
