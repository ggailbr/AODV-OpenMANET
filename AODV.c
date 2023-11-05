
#include "manet_testbed.h"
#include <stdio.h>
#include "AODV.h"
#include "recv_messages.h"
#include "send_messages.h"
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

uint8_t incoming_message(uint8_t *raw_pack, uint32_t src, uint32_t dest, uint8_t *payload, uint32_t payload_length);
uint8_t outgoing_message(uint8_t *raw_pack, uint32_t src, uint32_t dest, uint8_t *payload, uint32_t payload_length);
uint8_t forwarded_messages(uint8_t *raw_pack, uint32_t src, uint32_t dest, uint8_t *payload, uint32_t payload_length);

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
    SetInterface((uint8_t *)"wlan0");
    ip_address = GetInterfaceIP((uint8_t *)"wlan0", 0);
    // Register our functions
    if(RegisterIncomingCallback(incoming_message) != 0){
        printf("Error Registering Callback\n");
    }
    if(RegisterOutgoingCallback(outgoing_message) != 0){
        printf("Error Registering Callback\n");
    }
    if(RegisterForwardCallback(forwarded_messages) != 0){
        printf("Error Registering Callback\n");
    }
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
uint8_t incoming_message(uint8_t *raw_pack, uint32_t src, uint32_t dest, uint8_t *payload, uint32_t payload_length){
    packet_type type = payload[0];
    switch(type){
        case(RREQ_TYPE):
            return recv_rreq(src, (rreq_header *) payload);
            break;
        case(RREP_TYPE):
            return recv_rrep(src, (rrep_header *) payload);
            break;
        case(RERR_TYPE):
            return recv_rerr(src, payload);
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

uint8_t outgoing_message(uint8_t *raw_pack, uint32_t src, uint32_t dest, uint8_t *payload, uint32_t payload_length){
    // Check if we have a route already
    routing_entry * destination = get_routing_entry(routes, dest);
    if(destination != NULL && destination->status == ROUTE_VALID){
        pthread_mutex_lock(&destination->entry_mutex);
        set_expiration_timer(destination, ACTIVE_ROUTE_TIMEOUT);
        pthread_mutex_unlock(&destination->entry_mutex);
        // Pass Packet
        return 1;
    }
    // Search for a route
    return send_rreq(dest);
}

// Here will be the logic for accepting and resetting timers for forwarded packets
uint8_t forwarded_messages(uint8_t *raw_pack, uint32_t src, uint32_t dest, uint8_t *payload, uint32_t payload_length){
    routing_entry * destination = get_routing_entry(routes, dest);
    routing_entry * src_entry = get_routing_entry(routes, src);
    routing_entry * dest_hop, *origin_hop, *unreachable_dest;
    // If we have a route
    if(destination != NULL){
        pthread_mutex_lock(&destination->entry_mutex);
        dest_hop = get_routing_entry(routes, destination->next_hop);
        // And the route is valid and thenext hop is valid
        if(destination->status == ROUTE_VALID && dest_hop != NULL && dest_hop->status == ROUTE_VALID){
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
            set_expiration_timer(destination, ACTIVE_ROUTE_TIMEOUT);
            pthread_mutex_unlock(&destination->entry_mutex);
            return PACKET_ACCEPT;
        }
        // RERR condition 1
        // I.E Link break in next hop of active route
        else if(destination->status == ROUTE_VALID && dest_hop != NULL){
            // [TODO] Attempt Link Repair
            // If link repair fails
            uint32_t size = 0, num_dests = 0;
            uint32_t single_rerr = 0;
            uint8_t broadcast_rerr = 0;
            pthread_mutex_lock(&dest_hop->entry_mutex);
            uint32_t * destination_list = get_all_entries(dest_hop->next_hop_for, &size);
            uint32_t *dest_ip_seq_list = (uint32_t *) malloc(sizeof(uint32_t) * 2 * (size + 1));
            dest_ip_seq_list[0] = dest_hop->dest_ip;
            if(dest_hop->seq_valid == SEQ_INVALID){
                debprintf("[RERR] Generated without valid sequence");
            }
            dest_ip_seq_list[1] = dest_hop->dest_seq;
            num_dests += 1;
            set_expiration_timer(dest_hop, DELETE_PERIOD);
            pthread_mutex_unlock(&dest_hop->entry_mutex);
            for(int i = 0; i < size; i++){
                unreachable_dest = get_routing_entry(routes, destination_list[i]);
                if(unreachable_dest != NULL){
                    pthread_mutex_lock(&unreachable_dest->entry_mutex);
                    /*
                    The RERR should contain those destinations that are part of
                    the created list of unreachable destinations and have a non-empty
                    precursor list.
                    */
                    if(unreachable_dest->precursor_list->first != NULL){
                        if(unreachable_dest->precursor_list->first == unreachable_dest->precursor_list->last && single_rerr == 0){
                            single_rerr = unreachable_dest->precursor_list->first->data;
                        }
                        else{
                            broadcast_rerr = 1;
                        }
                        num_dests++;
                        dest_ip_seq_list[((num_dests)*2)] = unreachable_dest->dest_ip;
                        dest_ip_seq_list[((num_dests)*2) + 1] = unreachable_dest->dest_seq + 1;
                        if(active_routes > 0){
                            active_routes--;
                        }
                    }
                    /*
                    1. The destination sequence number of this routing entry, if it
                        exists and is valid, is incremented for cases (i) and (ii) above,
                        and copied from the incoming RERR in case (iii) above.

                    2. The entry is invalidated by marking the route entry as invalid

                    3. The Lifetime field is updated to current time plus DELETE_PERIOD.
                        Before this time, the entry SHOULD NOT be deleted.
                    */
                    unreachable_dest->dest_seq += 1;
                    DeleteEntry(unreachable_dest->dest_ip, unreachable_dest->next_hop);
                    unreachable_dest->status = ROUTE_INVALID;
                    set_expiration_timer(unreachable_dest, DELETE_PERIOD);

                    pthread_mutex_unlock(&unreachable_dest->entry_mutex);
                }
            }
            // Should have a buffer of pairs that is 2*num_dests long
            
            uint8_t * rerr_buff = generate_rerr_message_buff(&size, 0, num_dests, dest_ip_seq_list);
            if(single_rerr != 0 && broadcast_rerr == 0){
                SendUnicast(single_rerr, rerr_buff, NULL);
            }  
            else{
                SendBroadcast(rerr_buff, NULL);
            }
            free(destination_list);
            free(dest_ip_seq_list);
            free(rerr_buff);
            pthread_mutex_unlock(&destination->entry_mutex);
            return PACKET_DROP;
        }
    }
    // RERR Condition 2
    // Destined for node which it does not have an active route for
    uint8_t * rerr_buff;
    uint32_t size = 0;
    if(destination == NULL){
        debprintf("[RERR] No Destination Entry");
        rerr_buff = generate_rerr_message(&size, 0, 1, dest, 0);
    }
    else{
        rerr_buff = generate_rerr_message(&size, 0, 1, dest, destination->dest_ip);
        pthread_mutex_unlock(&destination->entry_mutex);
    }
    SendBroadcast(rerr_buff, NULL);
    free(rerr_buff);
    return PACKET_DROP;
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