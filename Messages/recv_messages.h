#ifndef __RECV_MESSAGES_H__
#define __RECV_MESSAGES_H__


#include "AODV.h"

uint8_t recv_rreq(uint32_t ttl, uint32_t sender_ip, rreq_header * rreq_message);
uint8_t recv_rrep(uint32_t sender_ip, rrep_header * rrep_message);
#endif