#include "stdlib.h"

int atoi(const char *str) {
    int sign;
    unsigned int nbr;

    while (*str && isspace(*str))
        str++;
    sign = 1;
    if (*str && (*str == '-' || *str == '+')) {
        if (*str == '-')
            sign = -1;
        str++;
    }
    nbr = strtonbr(str);
    return (sign * nbr);
}