#include "stdlib.h"
#include <stdbool.h>

bool isprint(int c) {
    return (c >= 0x20 && c <= 0x7E);
}