#ifndef __DEBUG_H__
#define __DEBUG_H__
#include <stdio.h>

#ifdef DEBUG
#define debprintf(...)  fprintf(stdout, __VA_ARGS__); fflush(stdout);
#else
#define debprintf(...)
#endif

#endif