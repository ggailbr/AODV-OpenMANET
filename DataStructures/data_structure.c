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
#include <stdlib.h>
#include <string.h>

uint8_t add_entry_to_data_structure(data_header container, entry entry, uint32_t sort_variable){
    // Since it is just an array, add it based on the last couple bytes of uint32_t
    uint8_t index = sort_variable >> 24;
    //debprintf("For %08x : Index Found to be %08x\n", sort_variable, index);
    container[index] = entry;
    return index;
}
entry remove_entry_from_data_structure(data_header container, uint32_t search_variable){
    uint8_t index = search_variable >> 24;
    //debprintf("For %08x : Index Found to be %08x\n", search_variable, index);
    entry return_val = container[index];
    container[index] = NULL;
    return return_val;
}
entry find_entry_in_data_structure(data_header container, uint32_t search_variable){
    uint8_t index = search_variable >> 24;
    //debprintf("For %08x : Index Found to be %08x\n", search_variable, index);
    return container[index];
}
data_header create_data_header(){
    data_header return_val = (data_header) malloc(sizeof(entry) * 256);
    memset(return_val, 0, sizeof(entry) * 256);
    return return_val;
}