#include "stdlib.h"

unsigned int strtonbr(const char *str) {
    unsigned int n;

    n = 0;
    while (*str && isnbr(*str)) {
        n = 10 * n + (*str - '0');
        str++;
    }
    return (n);
}