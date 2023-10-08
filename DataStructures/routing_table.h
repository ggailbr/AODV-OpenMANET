#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include <stdint.h>
#include <stdio.h>
#include "data_structure.h"
#include "linked_list.h"

typedef enum route_status_en{
    NOTHING = -1,
    ROUTE_VALID = 0,
    ROUTE_INVALID = 1,
    ROUTE_UNCONFIRMED = 2
}route_status;

typedef enum seq_valid_en{
    SEQ_VALID,
    SEQ_INVALID
}seq_valid;

typedef struct{
    uint32_t dest_ip;
    uint32_t dest_seq;
    seq_valid seq_valid;
    uint32_t next_hop;
    uint32_t hop_count;
    route_status status;
    uint32_t time_out;
    linked_list *precursor_list;
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
 * @return If the operation was successful (-1 on error)
 */
char create_or_update_routing_entry(routing_table table, uint32_t dest_ip, uint32_t dest_seq, seq_valid valid_seq, uint32_t next_hop, uint32_t hop_count, uint32_t time_out);
/**
 * \brief Set the route status for the given destination
 * 
 * \param table The Routing table the entry is within
 * \param dest_ip The destination IP of the entry
 * \param status The new status for the route. Can be one of the following:
 *  - NOTHING : Returns the current status of the route
 *  - ROUTE_VALID : The route is now valid
 *  - ROUTE_INVALID : The route is invalid
 *  - ROUTE_UNCONFIRMED : The route has not yet been confirmed (default state of routes)
 * \return The previous (or current in case of NOTHING) status of the route
 */
route_status set_route_status(routing_table table, uint32_t dest_ip, route_status status);
/**
 * \brief Move an entry from one table to another
 * 
 * @param initial The table containing the entry to move
 * @param destination The table to move the entry to
 * @param  dest_ip The destination IP of the entry
 * @return If the moving was a success 0. If it overwrote an entry in new table (1)
 */
char move_routing_entry(routing_table initial, routing_table destination, uint32_t dest_ip);

/**
 * \brief Set the hop_count of the route
 * 
 * \param table The Routing table the entry is within
 * \param dest_ip The destination IP of the entry
 * \param hop_count The new hop count. Use -1 to read
 * \return The previous (or current in case of -1) hop_count
 */
uint32_t set_hop_count(routing_table table, uint32_t dest_ip, uint32_t hop_count);

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

// Header [TODO]
int8_t compare_sequence_numbers(routing_table table, uint32_t dest_ip, uint32_t seq_num, uint8_t *error);


#endif // ROUTING_TABLE_H