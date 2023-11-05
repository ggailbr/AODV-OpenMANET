#include "routing_table.h"
#include <stdlib.h>
#include <time.h>
#include "time_funcs.h"

uint8_t add_routing_entry(routing_table table, routing_entry *routing_entry);
routing_entry * remove_routing_entry(routing_table table, uint32_t dest_ip);

routing_table create_routing_table(){
    return (routing_table) create_data_header();
}

/**
 * @brief Frees the routing entry
 * 
 * @param r_entry The entry to be freed
 */
void free_entry(routing_entry *r_entry){
    //pthread_cancel(r_entry->expiration_thread);
    pthread_cancel(r_entry->rreq_id_thread);
    pthread_cancel(r_entry->rreq_message_sender);
    pthread_mutex_destroy(&r_entry->entry_mutex);
    free_linked_list(r_entry->precursor_list);
    free_linked_list(r_entry->next_hop_for);
    free(r_entry);
}

routing_entry * create_or_get_routing_entry(routing_table table, uint32_t dest_ip, uint32_t dest_seq, seq_valid valid_seq, uint32_t next_hop, uint32_t hop_count, uint32_t time_out, uint8_t *new){
    routing_entry * new_entry;
    // First Check if one already exists
    if((new_entry = get_routing_entry(table, dest_ip)) != NULL){
        *new = 0;
        return new_entry;
    }
    *new = 1;
    new_entry = (routing_entry *) malloc(sizeof(routing_entry));
    pthread_mutex_init(&new_entry->entry_mutex, NULL);
    new_entry->dest_ip = dest_ip;
    new_entry->dest_seq = dest_seq;
    new_entry->next_hop = next_hop;
    new_entry->hop_count = hop_count;
    new_entry->seq_valid = valid_seq;
    new_entry->status = ROUTE_INVALID;
    new_entry->rreq_search = SEARCH_NONE;
    clock_gettime(CLOCK_REALTIME, &new_entry->time_out);
    add_time_ms(&new_entry->time_out,time_out);
    new_entry->precursor_list = (linked_list *) malloc(sizeof(linked_list));
    new_entry->precursor_list->first = NULL;
    new_entry->precursor_list->last = NULL;
    new_entry->next_hop_for = (linked_list *) malloc(sizeof(linked_list));
    new_entry->next_hop_for->first = NULL;
    new_entry->next_hop_for->last = NULL;
    add_routing_entry(table, new_entry);
    return new_entry;
}

char move_routing_entry(routing_table initial, routing_table destination, uint32_t dest_ip){
    routing_entry *moving_entry = remove_routing_entry(initial, dest_ip);
    routing_entry *destination_entry = remove_routing_entry(destination, dest_ip);
    if(add_entry_to_data_structure(destination, moving_entry, dest_ip) >= 0){
        if(destination_entry == NULL){
            return 0;
        }
        else{
            pthread_cancel(destination_entry->expiration_thread);
            free_entry(destination_entry);
            return 1;
        }
    }
    else{
        debprintf("[ERROR] : Unable to move entry");
        return -1;
    }
}

/**
 * @brief Interfaces between data structure and higher level function. 
 *  Adds a routing entry to the table
 * 
 * @param table The table to add the entry to
 * @param r_entry The entry to add
 * @return The success of the operation (index of entry)
 */
uint8_t add_routing_entry(routing_table table, routing_entry *r_entry){
    if(r_entry == NULL){
        debprintf("[ERROR] : Attempted to add NULL entry\n");
        return -1;
    }
    return add_entry_to_data_structure(table, (void *)r_entry, r_entry->dest_ip);
}
/**
 * @brief Interfaces between data structure and higher level function. 
 *  Removes a routing entry from the table
 * 
 * @param table The table to remove the entry
 * @param dest_ip The destination IP of the entry
 * @return A pointer to the removed entry
 */
routing_entry *remove_routing_entry(routing_table table, uint32_t dest_ip){
    return (routing_entry *) remove_entry_from_data_structure(table, dest_ip);
}

routing_entry *get_routing_entry(routing_table table, uint32_t dest_ip){
    return (routing_entry *) find_entry_in_data_structure(table, dest_ip);
}

// -------------Declare routing table specific functions--------------------

/**
 * @brief When a routing table entry is supposed to expire. It is a thread that
 *  will wait until the route timeout. After such, it will either delete the invalid
 *  route or will mark itself as invalid
 * 
 * @param own_entry The entry to expire on a timer
 */
void * expiration_func(void * thread_entry){
    // Find the difference in current time and expiration time
    routing_entry * own_entry = (routing_entry *)thread_entry;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    subtract_time(&current_time, &own_entry->time_out);
    // Wait until the route should expire
    while(nanosleep(&current_time, &current_time));
    // If it expires, grab the lock
    pthread_mutex_lock(&own_entry->entry_mutex);
    // If already invalid, delete itself
    if(own_entry->status == ROUTE_INVALID){
        pthread_mutex_unlock(&own_entry->entry_mutex);
        free_entry(remove_routing_entry(routes, own_entry->dest_ip));
        return NULL;
    }
    // Otherwise, set as invalid and restart with deleting
    own_entry->status = ROUTE_INVALID;
    // Delete Routing Table Entry
    DeleteEntry(own_entry->dest_ip, own_entry->next_hop);

    if(active_routes > 0 && own_entry->seq_valid == SEQ_VALID){
        active_routes--;
    }

    // Restart with delete timer
    clock_gettime(CLOCK_REALTIME, &own_entry->time_out);
    add_time_ms(&own_entry->time_out, DELETE_PERIOD);
    pthread_mutex_unlock(&own_entry->entry_mutex);
    // Calling with the delete period
    expiration_func(own_entry);
    return NULL;
}

/**
 * @brief Waits for the rreq_id buffer time before setting it to 0
 * 
 * @param own_entry The entry to monitor the RREQ_ID
 */
void * rreq_id_func(void * thread_entry){
    routing_entry * own_entry = (routing_entry *)thread_entry;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    struct timespec current_time;
    convert_ms_to_timespec(&current_time, PATH_DISCOVERY_TIME);
    while(nanosleep(&current_time, &current_time));
    pthread_mutex_lock(&own_entry->entry_mutex);
    own_entry->rreq_id = 0;
    own_entry->rreq_id_thread = 0;
    pthread_mutex_unlock(&own_entry->entry_mutex);
    return NULL;
}