#include "routing_table.h"
#include <stdlib.h>

uint8_t add_routing_entry(routing_table table, routing_entry *routing_entry);
routing_entry * remove_routing_entry(routing_table table, uint32_t dest_ip);
routing_entry *get_routing_entry(routing_table table, uint32_t dest_ip);

routing_table create_routing_table(){
    return (routing_table) create_data_header();
}

/**
 * \brief Frees the routing entry
 * 
 * @param r_entry The entry to be freed
 */
void free_entry(routing_entry *r_entry){
    free_linked_list(r_entry->precursor_list);
    free(r_entry);
}

char create_routing_entry(routing_table table, uint32_t dest_ip, uint32_t dest_seq, uint32_t next_hop, uint32_t hop_count, uint32_t time_out){
    // First Check if one already exists
    if(get_routing_entry(table, dest_ip) != NULL){
        debprintf("[ERROR] : Entry already initialized\n");
        return -1;
    }
    routing_entry * new_entry = (routing_entry *) malloc(sizeof(routing_entry));
    new_entry->dest_ip = dest_ip;
    new_entry->dest_seq = dest_seq;
    new_entry->next_hop = next_hop;
    new_entry->hop_count = hop_count;
    new_entry->status = ROUTE_UNCONFIRMED;
    new_entry->time_out = time_out;
    new_entry->precursor_list = (linked_list *) malloc(sizeof(linked_list));
    add_routing_entry(table, new_entry);
    return 0;
}

route_status set_route_status(routing_table table, uint32_t dest_ip, route_status status){
    routing_entry *dest_entry = get_routing_entry(table, dest_ip);
    if(dest_entry == NULL){
        return NOTHING;
    }
    else if(status == NOTHING){
        return dest_entry->status;
    }
    route_status previous = dest_entry->status;
    dest_entry->status = status;
    return previous;
}

char move_routing_entry(routing_table initial, routing_table destination, uint32_t dest_ip){
    entry *moving_entry = remove_routing_entry(initial, dest_ip);
    entry *destination_entry = remove_routing_entry(destination, dest_ip);
    if(add_entry_to_data_structure(destination, moving_entry, dest_ip) >= 0){
        if(destination_entry == NULL){
            return 0;
        }
        else{
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
 * \brief Interfaces between data structure and higher level function. 
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
 * \brief Interfaces between data structure and higher level function. 
 *  Removes a routing entry from the table
 * 
 * @param table The table to remove the entry
 * @param dest_ip The destination IP of the entry
 * @return A pointer to the removed entry
 */
routing_entry *remove_routing_entry(routing_table table, uint32_t dest_ip){
    return (routing_entry *) remove_entry_from_data_structure(table, dest_ip);
}
/**
 * \brief Interfaces between data structure and higher level function. 
 *  Finds a routing entry from the table
 * 
 * @param table The table to find the entry
 * @param dest_ip The destination IP of the entry
 * @return A pointer to the entry
 */
routing_entry *get_routing_entry(routing_table table, uint32_t dest_ip){
    return (routing_entry *) find_entry_in_data_structure(table, dest_ip);
}