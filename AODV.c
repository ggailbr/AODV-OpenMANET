
#include "manet_testbed.h"
#include <stdio.h>
#include "AODV.h"
#include "recv_messages.h"
#include "rrep.h"
#include "routing_table.h"
#include <time.h>
#include "time_funcs.h"

// [CHANGE] Manually setting for now, should be changed to the dynamic interface IP
uint32_t ip_address = 0xC4A80007;
safe_32 rreq_id;
safe_32 sequence_num;
routing_table routes;
volatile uint32_t active_routes = 0;
/*
 * This is a protocol main function. This is run once at the start 
 * of route creation. 
*/

/*
Protocol Prototypes representing undelying functions not yet implemented
*/

#ifdef HELLO_MESSAGES
void hello_interval();
#endif

int main(int argc, char **argv){
    // Start routing table
    routes = create_routing_table();
    // Initialize own seq and rreq_id
    rreq_id.number = 0x0U;
    sequence_num.number = 0x0U;
    // Initilize mutex control on seq and rreqID
    pthread_mutex_init(&rreq_id.mutex, NULL);
    pthread_mutex_init(&sequence_num.mutex, NULL);
    // Start the API
    InitializeAPI();
    // Set interface
    SetInterface("wlan0");
    ip_address = GetInterfaceIP("wlan0", 0);
    // Register our functions
    printf("Test\n");
    // Spin forever
    while(1);
    // If somehow leave, destroy mutex
    pthread_mutex_destroy(&rreq_id.mutex);
    pthread_mutex_destroy(&sequence_num.mutex);
    return 0;
}

/* 
 * This function is called each time a message is received
*/
int incoming_message(uint32_t sender_ip, uint8_t *body){
    packet_type type = body[0];
    switch(type){
        case(RREQ_TYPE):
            return recv_rreq(sender_ip, (rreq_header *) body);
            break;
        case(RREP_TYPE):
            return recv_rrep(sender_ip, (rrep_header *) body);
            break;
        case(RERR_TYPE):
            return recv_rerr(sender_ip, body);
            break;
        case(RREP_ACK_TYPE):
            // UNIMPLEMENTED
            // Mark Route as Bidirectional
            break;
        default:
            debprintf("[ERROR] : Unknown Packet Type");
    }
    return 0;
}

int outgoing_message(uint32_t dest_ip, uint8_t *body){
    // Check if we have a route already
    routing_entry * destination = get_routing_entry(routes, dest_ip);
    if(destination != NULL && destination->status == ROUTE_VALID){
        pthread_mutex_lock(&destination->entry_mutex);
        set_expiration_timer(destination, ACTIVE_ROUTE_TIMEOUT);
        pthread_mutex_unlock(&destination->entry_mutex);
        // Pass Packet
        return 1;
    }
    // Search for a route
    return send_rreq(dest_ip);
}

// Here will be the logic for accepting and resetting timers for forwarded packets
int forwarded_messages(uint32_t src_ip, uint32_t dest_ip, uint8_t *body){
    routing_entry * destination = get_routing_entry(routes, dest_ip);
    routing_entry * src_entry = get_routing_entry(routes, src_ip);
    routing_entry * dest_hop, *origin_hop;
    // If we have a route
    if(destination != NULL){
        pthread_mutex_lock(&destination->entry_mutex);
        dest_hop = get_routing_entry(routes, destination->next_hop);
        // And the route is valid and thenext hop is valid
        if(destination->status = ROUTE_VALID && dest_hop != NULL && dest_hop->status == ROUTE_VALID){
            // Updating all of the timers
            set_expiration_timer(destination, ACTIVE_ROUTE_TIMEOUT);
            if(src_entry != NULL){
                pthread_mutex_lock(&src_entry->entry_mutex);
                set_expiration_timer(src_entry, ACTIVE_ROUTE_TIMEOUT);
                if((origin_hop = get_routing_entry(routes, src_entry->next_hop)) != NULL){
                    pthread_mutex_lock(&origin_hop->entry_mutex);
                    set_expiration_timer(origin_hop, ACTIVE_ROUTE_TIMEOUT);
                    pthread_mutex_unlock(&origin_hop->entry_mutex);
                }
                pthread_mutex_unlock(&src_entry->entry_mutex);
            }
            if((dest_hop = get_routing_entry(routes, src_entry->next_hop)) != NULL){
                pthread_mutex_lock(&dest_hop->entry_mutex);
                set_expiration_timer(dest_hop, ACTIVE_ROUTE_TIMEOUT);
                pthread_mutex_unlock(&dest_hop->entry_mutex);
            }
            set_expiration_timer(destination, ACTIVE_ROUTE_TIMEOUT);
        }
        // RERR condition 1
        // I.E Link break in next hop of active route
        else if(destination->status = ROUTE_VALID){
            // [TODO] Attempt Link Repair
            
            continue;
        }
        // RERR Condition 2
        // Destined for node which it does not have an active route for
        else{

        }
        pthread_mutex_unlock(&destination->entry_mutex);
    }
    // RERR Condition 2
    // Destined for node which it does not have an active route for
    else{

    }
    return 0;
}

#ifdef HELLO_MESSAGES
void hello_interval(){
    struct timespec current_time;
    uint32_t seq = read_safe(&sequence_num);   
    uint8_t *rrep_buf = generate_rrep_message(0, 0, ip_address, seq, 0x0, ALLOWED_HELLO_LOSS * HELLO_INTERVAL);
    rrep_header * rrep_message = (rrep_header *)rrep_buf;
    while(1){
        while(active_routes == 0);
        convert_ms_to_timespec(&current_time, HELLO_INTERVAL);
        while(nanosleep(&current_time, &current_time));
        seq = read_safe(&sequence_num);   
        rrep_message->dest_seq = seq;
        // Send it back along sender's path
        SendBroadcast(rrep_buf, NULL);
    }
    // Free the message buffer made
    free(rrep_buf);
}
#endif