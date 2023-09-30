#ifndef AODV_H
#define AODV_H

#include "safe.h"

// Configuration Parameters
//---------------------------------------------
// In ms
#define ACTIVE_ROUTE_TIMEOUT 3,000 // In ms
#define ALLOWED_HELLO_LOSS 2
#define RREQ_RETRIES 2
#define RREQ_RATELIMIT 10
#define TIMEOUT_BUFFER 2
#define TTL_START 1
#define TTL_INCREMENT 2
#define TTL_THRESHOLD 7
#define HELLO_INTERVAL 1,000 // In ms
#define LOCAL_ADD_TTL 2
#define RERR_RATELIMIT 10
#define NET_DIAMETER 35
#define NODE_TRAVERSAL_TIME 40 // In ms
#define NET_TRAVERSAL_TIME 2 * NODE_TRAVERSAL_TIME * NET_DIAMETER
#define BLACKLIST_TIMEOUT RREQ_RETRIES * NET_TRAVERSAL_TIME
#define DELETE_PERIOD //[TODO]
#define MAX_REPAIR_TTL 0.3 * NET_DIAMETER
#define MIN_REPAIR_TTL //[TODO]
#define MY_ROUTE_TIMEOUT 2 * ACTIVE_ROUTE_TIMEOUT
#define NEXT_HOP_WAIT NODE_TRAVERSAL_TIME + 10
#define PATH_DISCOVERY_TIME 2 * NET_TRAVERSAL_TIME
#define TTL_VALUE //[TODO]
#define RING_TRAVERSAL_TIME 2 * NODE_TRAVERSAL_TIME * (TTL_VALUE + TIMEOUT_BUFFER)
//---------------------------------------------

#define RREQ_TYPE 1u
#define RREP_TYPE 2u
#define RERR_TYPE 3u
#define RREP_ACK_TYPE 4u
#define HELLO_TYPE 1u


extern uint32_t ip_address;
extern safe_32 rreq_id;
extern safe_32 sequence_num;


#endif