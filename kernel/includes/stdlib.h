#ifndef STDLIB_H
#define STDLIB_H
#include <stdbool.h>
int atoi(const char *nptr);
unsigned int strtonbr(const char *str);
int isnbr(char c);
int isspace(char c);
int toupper(int c);
bool isprint(int c);
// TODO reorganise this a little xd
#endif