#ifndef SAFE_H
#define SAFE_H
#include <stdint.h>
#include <pthread.h>

typedef struct safe_s{
    uint32_t number;
    pthread_mutex_t mutex;
}safe_32;

uint32_t increment_safe(safe_32 *target);
uint32_t read_safe(safe_32 *target);
uint32_t write_safe(safe_32 *target, uint32_t new_number);

#endif