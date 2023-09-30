#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>

#ifdef DEBUG
#define dprintf(...)  fprintf(stderr, __VA_ARGS__)
#else
#define dprintf(...)
#endif

#endif