#include "stdlib.h"

#include <limits.h>
unsigned int strtonbr(const char *str) {
    unsigned int n;
    n = 0;
    while (*str && isnbr(*str)) {
        unsigned int digit = (unsigned int)(*str - '0');
        if (n > UINT_MAX / 10 || (n == UINT_MAX / 10 && digit > UINT_MAX % 10)) {
            /* Saturate on overflow and consume remaining digits */
            n = UINT_MAX;
            do {
                str++;
            } while (*str && isnbr(*str));
            break;
        }
        n = n * 10 + digit;
        str++;
    }
    return n;
}