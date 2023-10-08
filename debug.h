#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>

#ifdef DEBUG
#define debprintf(...)  fprintf(stderr, __VA_ARGS__)
#else
#define debprintf(...)
#endif

#define max(x, y) x>y?x:y
#define seq_compare(x, y) (int32_t)x - (int32_t)y
#endif