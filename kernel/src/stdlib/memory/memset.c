#include "stdlib.h"

void *memset(void *s, int c, size_t n) {
    unsigned char *ptr = (unsigned char *)s;
    for (size_t i = 0; i < n; i++) {
        ptr[i] = (unsigned char)c;
    }
    return s;
}