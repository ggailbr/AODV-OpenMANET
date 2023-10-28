#ifndef __THREAD_FUNCS_H__
#define __THREAD_FUNCS_H__

#include <pthread.h>
#include <time.h>
#include "routing_table.h"
#include "AODV.h"

void add_time(struct timespec *current_time, struct timespec *end_time);
uint32_t get_ms_difference(struct timespec *current_time, struct timespec *end_time);
void subtract_time(struct timespec *current_time, struct timespec *end_time);
void add_time_ms(struct timespec *current_time, uint32_t miliseconds);
void convert_ms_to_timespec(struct timespec *current_time, uint32_t miliseconds);
uint32_t convert_timespec_to_ms(struct timespec *current_time);
void set_expiration_timer(routing_entry * entry, uint32_t ms);
void start_rreq_timer(routing_entry * entry);
struct timespec * max_timespec(struct timespec *current_time, struct timespec *end_time);
#endif