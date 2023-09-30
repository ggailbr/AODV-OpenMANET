#include "safe.h"
#include <pthread.h>

uint32_t increment_safe(safe_32 *target){
    uint32_t ret_val;
    pthread_mutex_lock(target->mutex);
    target->number++;
    ret_val = target->number;
    pthread_mutex_unlock(target->mutex);
    return ret_val;
}
uint32_t read_safe(safe_32 *target){
    uint32_t ret_val;
    pthread_mutex_lock(target->mutex);
    ret_val = target->number;
    pthread_mutex_unlock(target->mutex);
    return ret_val;
}
void write_safe(safe_32 *target, uint32_t new_number){
    pthread_mutex_lock(target->mutex);
    target->number = new_number;
    pthread_mutex_unlock(target->mutex);
}