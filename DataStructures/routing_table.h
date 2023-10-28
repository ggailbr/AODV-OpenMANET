#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include "data_structure.h"
#include "linked_list.h"

typedef enum route_status_en{
    ROUTE_VALID = 0,
    ROUTE_INVALID = 1,
}route_status;

typedef enum seq_valid_en{
    SEQ_VALID,
    SEQ_INVALID
}seq_valid;

typedef struct{
    pthread_mutex_t entry_mutex;
    uint32_t dest_ip;
    uint32_t dest_seq;
    seq_valid seq_valid;
    uint32_t next_hop;
    uint32_t hop_count;
    route_status status;
    struct timespec time_out;
    linked_list *precursor_list;
    pthread_t expiration_thread;
    pthread_t rreq_thread;
    pthread_t delete_thread;

} routing_entry;

typedef data_header routing_table;

/**
 * \brief Creates a routing entry for the destination in the provided table.
 * 
 * @param table The Routing Table for the entry to be stored
 * @param dest_ip The destination IP of the entry
 * @param dest_seq The destination sequence number
 * @param valid_seq If the sequence number is valid
 * @param next_hop The next hop towards the destination
 * @param hop_count The current number of hops to the destination
 * @param time_out The time for this route to expire
 * @return The routing entry location (NULL on error)
 */
routing_entry * create_or_get_routing_entry(routing_table table, uint32_t dest_ip, uint32_t dest_seq, seq_valid valid_seq, uint32_t next_hop, uint32_t hop_count, uint32_t time_out);

/**
 * \brief Makes the base object to hold the routing table
 * 
 * @return A routing_table variable to represent the table
 */
routing_table create_routing_table();
/**
 * \brief Interfaces between data structure and higher level function. 
 *  Finds a routing entry from the table
 * 
 * @param table The table to find the entry
 * @param dest_ip The destination IP of the entry
 * @return A pointer to the entry
 */
routing_entry *get_routing_entry(routing_table table, uint32_t dest_ip);

routing_entry * remove_routing_entry(routing_table table, uint32_t dest_ip);

void free_entry(routing_entry *r_entry);

#endif // ROUTING_TABLE_H