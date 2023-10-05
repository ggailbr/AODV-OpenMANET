#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include <stdint.h>
#include <stdio.h>
#include "data_structure.h"
#include "linked_list.h"

#define ROUTE_VALID 0
#define ROUTE_INVALID 1
#define ROUTE_UNCONFIRMED 2

typedef struct{
    uint32_t dest_ip;
    uint32_t dest_seq;
    uint32_t next_hop;
    uint32_t hop_count;
    uint8_t status;
    uint32_t time_out;
    linked_list *precursor_list;
} routing_entry;

typedef data_header routing_table;

routing_table create_routing_table();
uint8_t add_routing_entry(routing_table * table, routing_entry *routing_entry);
uint8_t remove_routing_entry(routing_table * table, routing_entry *routing_entry);
routing_entry *get_routing_entry(routing_table * table, uint32_t dest_ip);



#endif // ROUTING_TABLE_H