#include "api.h"
#include <stdio.h>
#include "AODV.h"
// [CHANGE] Manually setting for now, should be changed to the dynamic interface IP
uint32_t ip_address = 0xC4A80007;
safe_32 rreq_id;
safe_32 sequence_num;
routing_table routes;
/*
 * This is a protocol main function. This is run once at the start 
 * of route creation. 
*/

/*
Protocol Prototypes representing undelying functions not yet implemented
*/


int main(int argc, char **argv){
    routes = create_routing_table();
    rreq_id.number = 0x0U;
    sequence_num.number = 0x0U;

    pthread_mutex_init(&rreq_id.mutex, NULL);
    pthread_mutex_init(&sequence_num.mutex, NULL);
    InitializeApi();
    printf("Test\n");
    while(1);
    pthread_mutex_destroy(&rreq_id.mutex);
    pthread_mutex_destroy(&sequence_num.mutex);
    return 0;
}

/* 
 * This function is called each time a message is received by the 
 *
 * 
*/
int incoming_message(uint8_t *buf){
    packet_type type = get_packet_type(buf);
    uint32_t lenth = get_packet_length(buf);
    uint32_t sender_ip = get_packet_sender(buf);
    uint32_t ttl = get_packet_ttl(buf);
    switch(type){
        case(RREQ_TYPE):
            /*
                On Incoming RREQ:
                1) Create an entry for the previous hop
                2) Increment the hop count of the message
                3) Create/update a reverse route back to origin
                4) Check for duplicate RREQID/IP
                    4a) If duplicate, but destination look for lower hop count
                5) If destination, send RREP
                6) Check for "fresh enough" route to destination
                7) If no fresh route, check TTL and forward
            */

            // Converting Packet Body into RREQ Message
            ;
            rreq_header *message = (rreq_header *) buf;

            // Create route to previous hop
            // [TODO] Update Timeout/entry
            routing_entry * previous_hop = create_or_get_routing_entry(routes, sender_ip, 0, SEQ_INVALID, sender_ip, 0, MY_ROUTE_TIMEOUT);
            update_routing_entry(previous_hop, sender_ip, 0, SEQ_INVALID, 0);
            // [TODO]
            set_expiration_timer(previous_hop, ACTIVE_ROUTE_TIMEOUT);

            // Increment Hop Count
            increment_hop_rreq(buf);
            uint32_t hop_count = message->hop_count;

            // Create/Update Reverse Route
            // [TODO] Update Timeout
            routing_entry * origin_entry = create_or_get_routing_entry(routes, message->src_ip, message->src_seq, SEQ_VALID, sender_ip, message->hop_count, MY_ROUTE_TIMEOUT);
            update_routing_entry(origin_entry, sender_ip, message->src_seq, SEQ_VALID, hop_count - 1);
            // [TODO]
            // Set to the maximum of existing and Minimal Lifetime
            set_expiration_timer(origin_entry, ACTIVE_ROUTE_TIMEOUT);
            origin_entry->status = ROUTE_VALID;

            // Check if it was recently seen
            if(is_in_rreq_table(rreq_ids, message->src_ip, message->rreq_id)){
                // If it was recently seen, but this is the destination and the hop count is lower
                if(message->dest_ip == ip_address && message->hop_count < origin_entry->hop_count){
                    // [TODO] Update routing entry
                }
                return 0;
            }
            
            // Check if we are the destination
            if(message->dest_ip == ip_address){
                set_route_status(routes, message->src_ip, ROUTE_VALID);
                // // I.E the new hopcount was smaller than the previous one
                // // OR the current hop count is the only one (just got the message)
                // if(origin_entry->hop_count == hop_count){
                //     send_rrep_destination(message);
                // }
                return 0;
            }
            uint8_t error;
            // Check if has "fresh enough" route
            // [TODO]
            if(!(message->flags && RREQ_DEST_ONLY) && 
                compare_sequence_numbers(routes, message->dest_ip, message->dest_seq, &error) > 0 &&
                set_route_status(routes, message->dest_ip, NOTHING) == ROUTE_VALID){
                add_rreq_entry(rreq_ids, message->src_ip, message->rreq_id);
                send_rrep_intermediate(message, sender_ip);
                return 0;
            }
            if(ttl <= 1){
                return 0;
            }
            ttl--;
            if(compare_sequence_numbers(routes, message->dest_ip, message->dest_seq, &error) > 0){
                message->dest_seq = get_routing_entry(routes, message->dest_ip)->dest_seq;
            }
            // [TODO] broadcast RREQ with updated ttl
            SendBroadcast(buf, NULL);
            return 0;
            break;
        case(RREP_TYPE):
            rrep_header *message = (rrep_header *) buf;
            // Creating or updating route to prev hop
            create_or_update_routing_entry(routes, sender_ip, 0, SEQ_INVALID, sender_ip, 0, MY_ROUTE_TIMEOUT);
            // Increasing the hop counter
            increment_hop_rrep(message);
            // Creating route to Destination (originator of RREP/target of RREQ)
            create_or_update_routing_entry(routes, message->dest_ip, message->dest_seq, SEQ_VALID, sender_ip, message->hop_count, MY_ROUTE_TIMEOUT);
            // Setting both forward and reverse routes to valid
            set_route_status(routes, message->dest_ip, ROUTE_VALID);
            set_route_status(routes, message->src_ip, ROUTE_VALID);
            // Adding the next hop to each other precursor lists
            routing_entry * dest_entry = get_routing_entry(routes, message->dest_ip);
            routing_entry * origin_entry = get_routing_entry(routes, message->src_ip);
            add_entry_to_list(&dest_entry->precursor_list, origin_entry->next_hop);
            add_entry_to_list(&origin_entry->precursor_list, dest_entry->next_hop);
            // If we are the source IP address, kill the timing thread for RREQ and return
            if(message->src_ip == ip_address){
                // [TODO] Kill timing thread if present
                return 0;
            }
            // Send RREP to next hop
            SendUnicast(origin_entry->next_hop, buf, NULL);
            break;
        case(RERR_TYPE):
            break;
        case(RREP_ACK_TYPE):
            break;
        default:
            debprintf("[ERROR] : Unknown Packet Type");
    }
    return 0;
}

int outgoing_message(){

}