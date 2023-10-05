#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <stdint.h>
#include <stdio.h>

/*
    This is to allow flexible backend data structures
    for the routing table. This is the current limitation
    of network size as I am implementing it as an array, 
    however, the interface was designed for easy changing
    of this data structure
*/
typedef void* data_header;
typedef void *entry;

// Basic Interfacing functions
uint8_t add_entry_to_data_structure(data_header container, entry *entry, uint32_t sort_variable, size_t entry_bytes);
uint8_t remove_entry_from_data_structure(data_header container, entry *entry, uint32_t search_variable);
uint8_t find_entry_in_data_structure(data_header container, entry *entry, uint32_t search_variable);
data_header create_data_header();

#endif