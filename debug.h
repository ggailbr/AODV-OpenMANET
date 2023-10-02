#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>

#ifdef DEBUG
#define debprintf(...)  fprintf(stderr, __VA_ARGS__)
#else
#define debprintf(...)
#endif

#endif