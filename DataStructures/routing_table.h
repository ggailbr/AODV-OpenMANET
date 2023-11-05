#ifndef __ROUTING_TABLE_H__
#define __ROUTING_TABLE_H__

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include "data_structure.h"
#include "linked_list.h"

typedef enum route_status_en{
    ROUTE_VALID = 1,
    ROUTE_INVALID = 0,
}route_status;

typedef enum seq_valid_en{
    SEQ_VALID = 1,
    SEQ_INVALID = 0
}seq_valid;

typedef enum rreq_status_en{
    SEARCH_FOUND = 1,
    SEARCH_FAILED = 0,
    SEARCH_NONE = 2,
    SEARCH_SEARCHING = 3
}rreq_status;

/**
 * @struct routing_entry
 * @brief Defines the parts stores in a routing entry
 * 
 * @var routing_entry::entry_mutex
 *  A mutex to control access to this routing entry
 * @var routing_entry::dest_ip
 *  The destination IP associated with the structure
 * @var routing_entry::dest_seq
 *  The known destination sequence number
 * @var routing_entry::seq_valid
 *  If the sequence number is valid
 * @var routing_entry::next_hop
 *  Where to forward the message to
 * @var routing_entry::hop_count
 *  Number of hops until the destination
 * @var routing_entry::status
 *  The current status of the route
 * @var routing_entry::time_out
 *  A structure containing the time the route will expire
 * @var routing_entry::precursor_list
 *  A linked list contining the precursors of the route
 * @var routing_entry::expiration_thread
 *  The thread that controls how long the route is valid/alive
 * @var routing_entry::rreq_id
 *  The RREQ ID associated with the entry
 * @var routing_entry::rreq_id_thread
 *  Keeps the RREQ_ID valid only for a certain amount of time
 * @var routint_entry::rreq_search
 *  The status of the search for route
 * @var routing_entry::rreq_message_sender
 *  Thread that sends RREQ messages until either timeout or RREP
 */
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
    linked_list *next_hop_for;
    pthread_t expiration_thread;
    uint32_t rreq_id;
    pthread_t rreq_id_thread;
    volatile rreq_status rreq_search;
    pthread_t rreq_message_sender;

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
 * @param[in] new If the routing entry is new or existing
 * @return The routing entry location (NULL on error)
 */
routing_entry * create_or_get_routing_entry(routing_table table, uint32_t dest_ip, uint32_t dest_seq, seq_valid valid_seq, uint32_t next_hop, uint32_t hop_count, uint32_t time_out, uint8_t *new);

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

void* expiration_func(void * thread_entry);

void* rreq_id_func(void * thread_entry);

#endif // ROUTING_TABLE_H