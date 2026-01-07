#pragma once
#include <stdbool.h>
#include <stddef.h>

int atoi(const char *nptr);
unsigned int strtonbr(const char *str);
int isnbr(char c);
int isspace(char c);
int toupper(int c);
bool isprint(int c);
void *memset(void *s, int c, size_t n);
// TODO reorganise this a little xd
