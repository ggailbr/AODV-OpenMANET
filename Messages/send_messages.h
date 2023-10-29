#ifndef SEND_MESSAGES_H
#define SEND_MESSAGES_H

#include "rrep.h"
#include "rreq.h"
#include "rerr.h"
#include "routing_table.h"
#include "AODV.h"

void send_rrep_destination(rreq_header *message, uint32_t sender_ip);
void send_rrep_intermediate(rreq_header *message, uint32_t sender);

#endif