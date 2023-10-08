#ifndef RREQ_TRACKER_H
#define RREQ_TRACKER_H

#include <stdint.h>
#include <stdio.h>
#include "data_structure.h"

/* Header [TODO]*/
typedef struct{
    uint32_t src_ip;
    uint32_t rreq_id;
} rreq_entry;

typedef data_header rreq_table;

rreq_table create_rreq_table();
uint8_t add_rreq_entry(rreq_table table, uint32_t src_ip, uint32_t rreq_id);
uint8_t remove_rreq_entry(rreq_table table, uint32_t src_ip);
uint8_t is_in_rreq_table(rreq_table table, uint32_t src_ip, uint32_t rreq_id);

#endif