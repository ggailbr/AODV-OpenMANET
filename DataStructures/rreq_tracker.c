#include "rreq_tracker.h"
#include <stdlib.h>


/* Header [TODO]*/
rreq_table create_routing_table(){
    return (rreq_table) create_data_header();
}

/* Header [TODO]*/
uint8_t add_rreq_entry(rreq_table table, uint32_t src_ip, uint32_t rreq_id){
    rreq_entry *new_entry = (rreq_entry *) malloc(sizeof(rreq_entry));
    new_entry->src_ip = src_ip;
    new_entry->rreq_id = rreq_id;
    return add_entry_to_data_structure(table, (void *)new_entry, src_ip);
}

/* Header [TODO]*/
uint8_t remove_rreq_entry(rreq_table table, uint32_t src_ip){
    free(remove_entry_from_data_structure(table, src_ip));
}
/* Header [TODO]*/
uint8_t is_in_rreq_table(rreq_table table, uint32_t src_ip, uint32_t rreq_id){
    rreq_entry *new_entry = find_entry_in_data_structure(table, src_ip);
    if(new_entry != NULL && new_entry->rreq_id == rreq_id){
        return 1;
    }
    return 0;
}