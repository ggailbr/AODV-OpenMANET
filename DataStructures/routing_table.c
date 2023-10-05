#include "routing_table.h"




uint8_t add_routing_entry(routing_table * table, routing_entry *routing_entry);
uint8_t remove_routing_entry(routing_table * table, routing_entry *routing_entry);
routing_entry *get_routing_entry(routing_table * table, uint32_t dest_ip);