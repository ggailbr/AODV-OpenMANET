#ifndef AODV_H
#define AODV_H

#include "safe.h"
#include "debug.h"
#include "routing_table.h"
#include "rerr.h"
#include "rreq.h"
#include "rrep.h"
#include "manet_testbed.h"
#include "send_messages.h"

// Configuration Parameters
//---------------------------------------------
// In ms
#define ACTIVE_ROUTE_TIMEOUT 3000 // In ms
#define ALLOWED_HELLO_LOSS 2
#define RREQ_RETRIES 2
#define RREQ_RATELIMIT 10
#define TIMEOUT_BUFFER 2
#define TTL_START 1
#define TTL_INCREMENT 2
#define TTL_THRESHOLD 7
#define HELLO_INTERVAL 1000 // In ms
#define LOCAL_ADD_TTL 2
#define RERR_RATELIMIT 10
#define NET_DIAMETER 35
#define NODE_TRAVERSAL_TIME 40 // In ms
#define NET_TRAVERSAL_TIME 2 * NODE_TRAVERSAL_TIME * NET_DIAMETER
#define BLACKLIST_TIMEOUT RREQ_RETRIES * NET_TRAVERSAL_TIME
#define DELETE_PERIOD 5 * HELLO_INTERVAL
#define MAX_REPAIR_TTL 0.3 * NET_DIAMETER
#define MIN_REPAIR_TTL //[TODO]
#define MY_ROUTE_TIMEOUT 2 * ACTIVE_ROUTE_TIMEOUT
#define NEXT_HOP_WAIT NODE_TRAVERSAL_TIME + 10
#define PATH_DISCOVERY_TIME 2 * NET_TRAVERSAL_TIME
#define TTL_VALUE //[TODO]
#define RING_TRAVERSAL_TIME 2 * NODE_TRAVERSAL_TIME * (TTL_VALUE + TIMEOUT_BUFFER)
//---------------------------------------------


#define max(x, y) x>y?x:y
/*
    In order to ascertain that information about a destination is not
   stale, the node compares its current numerical value for the sequence
   number with that obtained from the incoming AODV message.  This
   comparison MUST be done using signed 32-bit arithmetic, this is
   necessary to accomplish sequence number rollover.  If the result of
   subtracting the currently stored sequence number from the value of
   the incoming sequence number is less than zero, then the information
   related to that destination in the AODV message MUST be discarded,
   since that information is stale compared to the node's currently
   stored information.
*/
#define seq_compare(x, y) (int32_t)x - (int32_t)y

typedef enum packet_type_e{
    RREQ_TYPE = 1u,
    RREP_TYPE = 2u,
    RERR_TYPE = 3u,
    RREP_ACK_TYPE = 4u,
    HELLO_TYPE = 1u
}packet_type;


extern uint32_t ip_address;
extern safe_32 rreq_id;
extern safe_32 sequence_num;
extern routing_table routes;
extern uint32_t active_routes;

#endif