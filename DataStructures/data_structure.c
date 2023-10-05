/*
    Gage Gailbreath
    ggailbr@clemson.edu

    Purpose: 
        Implements the underlying data structure which holds the routing table for the
        AODV routing protocol. This was designed to obfuscate as much of the back end
        from the routing protocol to allow dynamic changes.

    Limitations:
        Currently, I am implementing it as a static array for 256 entries. This is because
        this protocol will only be tested on physical systems, of which, we currently only 
        have 12 Pis. Since these are much smaller scale tests, a simple array would allow
        fast search and addition times based on the last 8bytes of the address.
    Notes:
        Despite this nieve implementation, this should not have an impact on future development.
        The blanket interface was designed with things like hash tables in mind to allow for 
        more sophosticated structures.
*/

#include "data_structure.h"

uint8_t add_entry_to_data_structure(data_header container, entry *entry, uint32_t sort_variable, size_t entry_bytes){
    // Since it is just an array, add it based on the last couple bytes

}
uint8_t remove_entry_from_data_structure(data_header container, entry *entry, uint32_t search_variable);
uint8_t find_entry_in_data_structure(data_header container, entry *entry, uint32_t search_variable);
data_header create_data_header();