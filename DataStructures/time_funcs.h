#ifndef __THREAD_FUNCS_H__
#define __THREAD_FUNCS_H__

#include <pthread.h>
#include <time.h>
#include "routing_table.h"
#include "AODV.h"

void add_time(timespec *current_time, timespec *end_time);
uint32_t get_ms_difference(timespec *current_time, timespec *end_time);
void subtract_time(timespec *current_time, timespec *end_time);
void add_time_ms(timespec *current_time, uint32_t miliseconds);
void convert_ms_to_timespec(timespec *current_time, uint32_t miliseconds);
#endif