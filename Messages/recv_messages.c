#include <time.h>
#include "time_funcs.h"
#include "recv_messages.h"
#include "rreq.h"



uint8_t recv_rreq(uint32_t ttl, uint32_t sender_ip, rreq_header * rreq_message){
    // Create route to previous hop
    uint8_t new = 0;
    // Get entry to sender
    routing_entry * previous_hop = create_or_get_routing_entry(routes, sender_ip, 0, SEQ_INVALID, sender_ip, 0, ACTIVE_ROUTE_TIMEOUT, &new);
    // Set that as a valid route
    pthread_mutex_lock(&previous_hop->entry_mutex);
    previous_hop->status = ROUTE_VALID;
    // Add to the routing table
    AddUnicastRoutingEntry(sender_ip, sender_ip);
    // If this is an existing entry
    if(new == 0){
        clock_gettime(CLOCK_REALTIME, &previous_hop->time_out);
        add_time_ms(&previous_hop->time_out, ACTIVE_ROUTE_TIMEOUT);
    }
    // (Re)Start the expiration timer for the previous hop
    set_expiration_timer(previous_hop, ACTIVE_ROUTE_TIMEOUT);
    pthread_mutex_unlock(&previous_hop->entry_mutex);
    // If this is our RREQ
    if(rreq_message->src_ip == ip_address){
        return 0;
    }
    // Check for RREQ ID
    routing_entry * originator = get_routing_entry(routes, rreq_message->src_ip);
    // If we have an entry for the originator
    if(originator != NULL){
        // If we have already seen this RREQID, ignore it
        // Unless we are the destination
        if(originator->rreq_id == rreq_message->rreq_id && ip_address != rreq_message->dest_ip){
            return 0;
        }
        // Ignore if repeat and not better than current route
        else if(ip_address == rreq_message->dest_ip && originator->hop_count <= rreq_message->hop_count){
            return 0;
        }
    }
    // Increment the hop count
    increment_hop_rreq(rreq_message);

    // Search for the Reverse Route
    originator = create_or_get_routing_entry(routes, rreq_message->src_ip, rreq_message->src_seq, SEQ_VALID, sender_ip, rreq_message->hop_count, 0, &new);
    pthread_mutex_lock(&originator->entry_mutex);
    originator->rreq_id = rreq_message->rreq_id;
    start_rreq_timer(originator);
    // If a route currently exists
    if(new == 0){
        /*
            6.2
            The route is only updated if the new sequence number is either

            (i)       higher than the destination sequence number in the route
                        table, or

            (ii)      the sequence numbers are equal, but the hop count (of the
                        new information) plus one, is smaller than the existing hop
                        count in the routing table, or

            (iii)     the sequence number is unknown.
        */
        if(rreq_message->src_seq > originator->dest_seq || originator->seq_valid == SEQ_INVALID || (rreq_message->src_seq == originator->dest_seq && rreq_message->hop_count < originator->hop_count)){
            /*
            When the reverse route is
            created or updated, the following actions on the route are also
            carried out:

            1. the Originator Sequence Number from the RREQ is compared to the
                corresponding destination sequence number in the route table entry
                and copied if greater than the existing value there

            2. the valid sequence number field is set to true;

            3. the next hop in the routing table becomes the node from which the
                RREQ was received (it is obtained from the source IP address in
                the IP header and is often not equal to the Originator IP Address
                field in the RREQ message);

            4. the hop count is copied from the Hop Count in the RREQ message;
            */
            originator->dest_seq = rreq_message->src_seq;
            originator->seq_valid = SEQ_VALID;
            originator->next_hop = sender_ip;
            originator->hop_count = rreq_message->hop_count;
        }
    }
    else{
        originator->dest_seq = rreq_message->src_seq;
        originator->seq_valid = SEQ_VALID;
        originator->next_hop = sender_ip;
        originator->hop_count = rreq_message->hop_count;
    }
    struct timespec minimal_lifetime;
    clock_gettime(CLOCK_REALTIME, &minimal_lifetime);
    add_time_ms(&minimal_lifetime, 2*NET_TRAVERSAL_TIME - 2 * rreq_message->hop_count*NODE_TRAVERSAL_TIME);
    set_expiration_timer(originator, convert_timespec_to_ms(max_timespec(&minimal_lifetime, &originator->time_out)));
    pthread_mutex_unlock(&originator->entry_mutex);
    // If we are destination, now that we have installed the route, send a rrep and return
    if(ip_address == rreq_message->dest_ip){
        send_rrep_destination(rreq_message);
        return 0;
    }
    // Check for active route. If active and "d" not set, send from intermediate
    // [TODO]
    // If does not send a RREP


    return 0;
}

uint8_t recv_rrep(uint32_t sender_ip, rrep_header * rrep_message){
    return 0;
}