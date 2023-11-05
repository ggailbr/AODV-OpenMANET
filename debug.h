#ifndef __DEBUG_H__
#define __DEBUG_H__
#include <stdio.h>

#ifdef DEBUG
#define debprintf(...)  fprintf(stderr, __VA_ARGS__)
#else
#define debprintf(...)
#endif

#endif