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
void subtract_time(struct timespec *current_time, struct timespec *end_time){
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
uint32_t get_ms_difference(struct timespec *current_time, struct timespec *end_time){
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
void add_time(struct timespec *current_time, struct timespec *end_time){
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
void add_time_ms(struct timespec *current_time, uint32_t miliseconds){
    // current_time = End Time-CurrentTime
    current_time->tv_sec += miliseconds/1000;
    current_time->tv_nsec = (miliseconds - 1000*(miliseconds/1000))*1000000 + current_time->tv_nsec;
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
void convert_ms_to_timespec(struct timespec *current_time, uint32_t miliseconds){
    //debprintf("Convering %d to timestruct\n", miliseconds);
    current_time->tv_sec = miliseconds/1000;
    current_time->tv_nsec = (miliseconds - current_time->tv_sec*1000)*1000000;
    //debprintf("Timespec: {\n\t %ldsec \n\t%ld ns\n", current_time->tv_sec,  current_time->tv_nsec );
    while(current_time->tv_nsec > 1000000000){
        current_time->tv_nsec -= 1000000000;
        current_time->tv_sec++;
    }
}

/**
 * @brief Returns ms out of a timespec
 * 
 * @param current_time The structure memory to store the result
 * @param miliseconds The time in MS to store in the current_time
 */
uint32_t convert_timespec_to_ms(struct timespec *current_time){
    uint32_t ms = 0;
    ms += current_time->tv_sec * 1000;
    ms += current_time->tv_nsec/1000000;
    return ms;
}

/**
 * @brief Returns the maximum of the two timespec
 * 
 * @param current_time Timespec 1
 * @param end_time Timespec 2
 * @return Pointer to Timespec with time
 */
struct timespec * max_timespec(struct timespec *current_time, struct timespec *end_time){
    if(current_time->tv_sec >= end_time->tv_sec){
        if(current_time->tv_nsec >= end_time->tv_nsec)
            return current_time;
        else{
            if(current_time->tv_sec- (current_time->tv_nsec - end_time->tv_nsec)/1000000000 >= end_time->tv_sec)
                return current_time;
            else
                return end_time;
        }
    }
    return end_time;
}

/**
 * @brief Starts the expiration timer for the routing_entry
 * 
 * @param entry The entry to start the timer for
 * @param ms The time in ms. If 0, uses entry timeout. Otherwise,
 *  modifies the entry
 */
void set_expiration_timer(routing_entry * entry, uint32_t ms){
    // Lock the entry
    // pthread_mutex_lock(&entry->entry_mutex);
    // Stop Any Old Timer
    if(!(entry->expiration_thread == 0)){
        pthread_cancel(entry->expiration_thread);
        // debprintf("[SET_EXPIR] Canceling %ld\n", entry->expiration_thread);
        entry->expiration_thread = 0;
    }
    // Check if the user provided a time
    if(ms != 0){
        clock_gettime(CLOCK_REALTIME, &entry->time_out);
        add_time_ms(&entry->time_out, ms);
    }
    debprintf("[SET_EXPIR] Starting expiration for %x with timeout %d\n", entry->dest_ip, ms);
    // Start the new thread
    pthread_create(&entry->expiration_thread, NULL, expiration_func, (void *) entry);
    // pthread_mutex_unlock(&entry->entry_mutex);
}
/**
 * @brief Starts a timer to null the rreqid for an IP
 * 
 * @param entry The entry associated with the rreq ID
 */
void start_rreq_timer(routing_entry * entry){
    // Lock the entry
    // pthread_mutex_lock(&entry->entry_mutex);
    // Stop Any Old Timer
    if(!(entry->rreq_id_thread == 0)){
        pthread_cancel(entry->rreq_id_thread);
        // debprintf("[RREQ_TIM] Canceling %ld\n", entry->rreq_id_thread);
    }
    // Start the new thread
    pthread_create(&entry->rreq_id_thread, NULL, rreq_id_func, (void *) entry);
    // pthread_mutex_unlock(&entry->entry_mutex);
}