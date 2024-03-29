#ifndef __DATA_STRUCTURE_H__
#define __DATA_STRUCTURE_H__

#include <stdint.h>
#include <stdio.h>
#include "../debug.h"

/*
    This is to allow flexible backend data structures
    for the routing table. This is the current limitation
    of network size as I am implementing it as an array, 
    however, the interface was designed for easy changing
    of this data structure
*/
typedef void** data_header;
typedef void *entry;


// Basic Interfacing functions
uint8_t add_entry_to_data_structure(data_header container, entry entry, uint32_t sort_variable);
entry remove_entry_from_data_structure(data_header container, uint32_t search_variable);
entry find_entry_in_data_structure(data_header container, uint32_t search_variable);
data_header create_data_header();

#endif