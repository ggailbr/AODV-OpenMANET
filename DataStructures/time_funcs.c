#include <pthread.h>
#include <time.h>
#include "routing_table.h"
#include "AODV.h"
#include "time_funcs.h"

/**
 * @brief Subtracts the current_time from the end_time. Stores result in current_time
 * 
 * @param current_time The current time to subtract from end time
 * @param end_time The end time to compare against
 */
void subtract_time(timespec *current_time, timespec *end_time){
    // current_time = End Time-CurrentTime
    current_time->tv_sec = end_time->tv_sec - current_time->tv_sec;
    if(current_time->tv_sec < 0){
        current_time->tv_nsec = 0;
        current_time->tv_sec = 0;
        return;
    }
    current_time->tv_nsec = end_time->tv_nsec - current_time->tv_nsec;
    if(current_time->tv_nsec < 0){
        current_time->tv_nsec += 1000000000;
        current_time->tv_sec--;
    }
}
/**
 * @brief Returns the difference between two times in ms 
 * 
 * @param current_time The time to subtract 
 * @param end_time The time to subtract from
 * @return The difference in ms (returns 0 if below 0)
 */
uint32_t get_ms_difference(timespec *current_time, timespec *end_time){
    uint32_t ms_diff = 0;
    long end_time_nsec = end_time->tv_nsec;
    // If there is the current time is greater than the end time
    if(end_time->tv_sec - current_time->tv_sec < 0){
        return 0;
    }
    ms_diff += 1000*(end_time->tv_sec - current_time->tv_sec);
    // If we have positive seconds, but negative nsec
    if(end_time_nsec - current_time->tv_nsec < 0 && ms_diff > 0){
        ms_diff -= 1000;
        end_time_nsec += 1000000000;
    }
    ms_diff += (end_time_nsec - current_time->tv_nsec)/1000000;
    return ms_diff;
}

/**
 * @brief Adds the current time to the end time
 * 
 * @param current_time The current time to add, is modified by the function
 * @param end_time The time to add to
 */
void add_time(timespec *current_time, timespec *end_time){
    // current_time = End Time-CurrentTime
    current_time->tv_sec = end_time->tv_sec + current_time->tv_sec;
    current_time->tv_nsec = end_time->tv_nsec + current_time->tv_nsec;
    if(current_time->tv_nsec > 1000000000){
        current_time->tv_nsec -= 1000000000;
        current_time->tv_sec++;
    }
}

/**
 * @brief Adds the specified ms to the time
 * 
 * @param current_time The time to be added to
 * @param miliseconds The time in ms to add
 */
void add_time_ms(timespec *current_time, uint32_t miliseconds){
    // current_time = End Time-CurrentTime
    current_time->tv_nsec = miliseconds*1000000 + current_time->tv_nsec;
    while(current_time->tv_nsec > 1000000000){
        current_time->tv_nsec -= 1000000000;
        current_time->tv_sec++;
    }
}

/**
 * @brief Creates a timespec out of a ms amount
 * 
 * @param current_time The structure memory to store the result
 * @param miliseconds The time in MS to store in the current_time
 */
void convert_ms_to_timespec(timespec *current_time, uint32_t miliseconds){
    current_time->tv_sec = 0;
    current_time->tv_nsec = miliseconds*1000000;
    while(current_time->tv_nsec > 1000000000){
        current_time->tv_nsec -= 1000000000;
        current_time->tv_sec++;
    }
}