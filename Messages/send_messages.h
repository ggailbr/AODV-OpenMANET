#ifndef __SEND_MESSAGES_H__
#define __SEND_MESSAGES_H__

#include <stdint.h>
#include "AODV.h"
#include "rreq.h"

void send_rrep_destination(rreq_header *message, uint32_t sender_ip);
void send_rrep_intermediate(rreq_header *message, uint32_t sender);
uint8_t send_rreq(uint32_t dest_addr);

#endif