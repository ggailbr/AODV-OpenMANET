#include <time.h>
#include "time_funcs.h"
#include "recv_messages.h"
#include "rreq.h"



uint8_t recv_rreq(uint32_t sender_ip, rreq_header * rreq_message){
    // Create route to previous hop
    uint8_t new = 0;
    // Get entry to sender
    routing_entry * previous_hop = create_or_get_routing_entry(routes, sender_ip, 0, SEQ_INVALID, sender_ip, 0, ACTIVE_ROUTE_TIMEOUT, &new);
    // Set that as a valid route
    pthread_mutex_lock(&previous_hop->entry_mutex);
    previous_hop->status = ROUTE_VALID;
    // Adding the src to the next_hop list
    if(is_in_list(previous_hop->next_hop_for, rreq_message->src_ip) == 0){
        add_entry_to_list(previous_hop->next_hop_for, rreq_message->src_ip);
    }
    // Add to the routing table
    AddUnicastRoutingEntry(sender_ip, sender_ip);
    // If this is an existing entry, reset expiration
    if(new == 0){
        // May not need
        clock_gettime(CLOCK_REALTIME, &previous_hop->time_out);
        add_time_ms(&previous_hop->time_out, ACTIVE_ROUTE_TIMEOUT);
    }
    // (Re)Start the expiration timer for the previous hop
    set_expiration_timer(previous_hop, 0);
    pthread_mutex_unlock(&previous_hop->entry_mutex);

    // If this is our RREQ
    // Prevent retransmission of our own RREQ
    if(rreq_message->src_ip == ip_address){
        return LOGGING;
    }

    // Check for RREQ ID
    routing_entry * originator = get_routing_entry(routes, rreq_message->src_ip);
    // If we have an entry for the originator
    if(originator != NULL){
        // If we have already seen this RREQID, ignore it
        // Unless we are the destination
        if(originator->rreq_id == rreq_message->rreq_id && ip_address != rreq_message->dest_ip){
            return LOGGING;
        }
        // Ignore if repeat and not better than current route
        else if(originator->rreq_id == rreq_message->rreq_id &&ip_address == rreq_message->dest_ip && originator->hop_count <= rreq_message->hop_count){
            return LOGGING;
        }
    }

    // Increment the hop count
    increment_hop_rreq((uint8_t *)rreq_message);

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
    // Add to the routing table
    originator->status = ROUTE_VALID;
    AddUnicastRoutingEntry(sender_ip, sender_ip);
    // Calculate minimal time
    struct timespec minimal_lifetime, current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    convert_ms_to_timespec(&minimal_lifetime, 2*NET_TRAVERSAL_TIME - 2 * rreq_message->hop_count*NODE_TRAVERSAL_TIME);
    // Get existing lifetime
    subtract_time(&current_time, &originator->time_out);
    // Set Expiration to max of the two
    set_expiration_timer(originator, convert_timespec_to_ms(max_timespec(&minimal_lifetime, &current_time)));
    pthread_mutex_unlock(&originator->entry_mutex);

    // If we are destination, now that we have installed the route, send a rrep and return
    if(ip_address == rreq_message->dest_ip){
        send_rrep_destination(rreq_message, sender_ip);
        return LOGGING;
    }
    // Check for active route. If active and "d" not set, send from intermediate
    routing_entry * destination = get_routing_entry(routes, rreq_message->dest_ip);
    if((rreq_message->flags & RREQ_DEST_ONLY) == 0){
        /*  
            it has an active route to the destination, the destination
             sequence number in the node's existing route table entry
             for the destination is valid and greater than or equal to
             the Destination Sequence Number of the RREQ (comparison
             using signed 32-bit arithmetic), and the "destination only"
             ('D') flag is NOT set.
        */ 
        if(destination != NULL && destination->status == ROUTE_VALID && destination->seq_valid == SEQ_VALID && ((rreq_message->flags & RREQ_UNKNOWN) != 0 || seq_compare(rreq_message->dest_seq, destination->dest_seq) < 0)){
            send_rrep_intermediate(rreq_message, sender_ip);
            return LOGGING;
        }
    }
    // ---------------------------
    // | If does not send a RREP |
    // ---------------------------

    if(rreq_message->ttl <= 1){
        return LOGGING;
    }
    // Decrement ttl
    rreq_message->ttl--;
    // Hop Count incremented earlier

    // If we have a fresher destination sequence number
    // Or if the unknown flag is set
    if(destination != NULL && destination->seq_valid == SEQ_VALID && ((rreq_message->flags & RREQ_UNKNOWN) != 0 || seq_compare(rreq_message->dest_seq, destination->dest_seq) < 0))
        rreq_message->dest_seq = destination->dest_seq;
    SendBroadcast((uint8_t *)rreq_message, NULL);
    return LOGGING;
}

uint8_t recv_rrep(uint32_t sender_ip, rrep_header * rrep_message){
    // If we are the originator of rreq, immediately stop rreq thread
    routing_entry * destination = get_routing_entry(routes ,rrep_message->dest_ip);
    // (Can be NULL in the case of a HELLO message)
    if(ip_address == rrep_message->src_ip && destination != NULL){
        pthread_mutex_lock(&destination->entry_mutex);
        // Signal a route has been found
        destination->rreq_search = SEARCH_FOUND;
        pthread_cancel(destination->rreq_message_sender);
        destination->rreq_message_sender = 0;
        pthread_mutex_unlock(&destination->entry_mutex);
    }

    // Create route to previous hop
    uint8_t new = 0;
    // Check if this is a hello message
    if(rrep_message->src_ip != 0x0){
        // Get entry to sender
        routing_entry * previous_hop = create_or_get_routing_entry(routes, sender_ip, 0, SEQ_INVALID, sender_ip, 0, ACTIVE_ROUTE_TIMEOUT, &new);
        // Set that as a valid route
        pthread_mutex_lock(&previous_hop->entry_mutex);
        previous_hop->status = ROUTE_VALID;
        // Add to the routing table
        AddUnicastRoutingEntry(sender_ip, sender_ip);
        // Creating a List of where this is next hop for
        if(is_in_list(previous_hop->next_hop_for, rrep_message->dest_ip) == 0){
            add_entry_to_list(previous_hop->next_hop_for, rrep_message->dest_ip);
        }

        // If this is an existing entry, reset expiration
        if(new == 0){
            // May not need
            clock_gettime(CLOCK_REALTIME, &previous_hop->time_out);
            add_time_ms(&previous_hop->time_out, ACTIVE_ROUTE_TIMEOUT);
        }
        // (Re)Start the expiration timer for the previous hop
        set_expiration_timer(previous_hop, 0);
        pthread_mutex_unlock(&previous_hop->entry_mutex);
    }

    // Increment hop in rrep
    increment_hop_rrep((uint8_t *) rrep_message);

    //Create forward route
    destination = create_or_get_routing_entry(routes, rrep_message->dest_ip, rrep_message->dest_seq, SEQ_VALID, sender_ip, rrep_message->hop_count, rrep_message->lifetime, &new);
    pthread_mutex_lock(&destination->entry_mutex);
    /*Upon comparison,
        the existing entry is updated only in the following circumstances:

        (i)       the sequence number in the routing table is marked as
                    invalid in route table entry.

        (ii)      the Destination Sequence Number in the RREP is greater than
                    the node's copy of the destination sequence number and the
                    known value is valid, or

        (iii)     the sequence numbers are the same, but the route is is
                    marked as inactive, or

        (iv)      the sequence numbers are the same, and the New Hop Count is
                    smaller than the hop count in route table entry.
    */
    if(new == 1 
        || destination->seq_valid == SEQ_INVALID 
        || seq_compare(rrep_message->dest_seq, destination->dest_seq) >= 0 
        || (rrep_message->dest_seq == destination->dest_seq && destination->status == ROUTE_INVALID)
        || (rrep_message->dest_seq == destination->dest_seq && rrep_message->hop_count < destination->hop_count)){
    /*
     If the route table entry to the destination is created or updated,
   then the following actions occur:

    -  the route is marked as active,

    -  the destination sequence number is marked as valid,

    -  the next hop in the route entry is assigned to be the node from
        which the RREP is received, which is indicated by the source IP
        address field in the IP header,
        
    -  the hop count is set to the value of the New Hop Count,

    -  the expiry time is set to the current time plus the value of the
        Lifetime in the RREP message,

    -  and the destination sequence number is the Destination Sequence
        Number in the RREP message.
    */
        destination->status = ROUTE_VALID;
        destination->seq_valid = SEQ_VALID;
        destination->next_hop = sender_ip;
        destination->hop_count = rrep_message->hop_count;
        set_expiration_timer(destination, rrep_message->lifetime);
        destination->dest_seq = rrep_message->dest_seq;
    }
    // Otherwise, still reset the expiration timer
    else{
        set_expiration_timer(destination, rrep_message->lifetime);
    }
    destination->status = ROUTE_VALID;
    AddUnicastRoutingEntry(rrep_message->dest_ip, sender_ip);

    // If this is a hello message
    if(rrep_message->src_ip == 0x0){
        pthread_mutex_unlock(&destination->entry_mutex);
        // If we require an ack
        if((rrep_message->flags & RREP_ACK) != 0){
            //[TODO]
            return LOGGING;
        }
        return LOGGING;
    }
    else{
        active_routes++;
    }
    
    // If we are not the originator of the rreq, add precursors
    if(ip_address != rrep_message->src_ip){
        routing_entry * originator = get_routing_entry(routes, rrep_message->src_ip);
        if(originator == NULL){
            debprintf("ERROR [RREP]Unknown Origin on RREP\n");
        }
        else{
            pthread_mutex_lock(&originator->entry_mutex);
            add_entry_to_list(originator->precursor_list, destination->next_hop);
            add_entry_to_list(destination->precursor_list, originator->next_hop);
            SendUnicast(originator->next_hop, (uint8_t *) rrep_message, NULL);
            pthread_mutex_unlock(&originator->entry_mutex);
        }
    }
    pthread_mutex_unlock(&destination->entry_mutex);
    // If we need to acknowledge
    if((rrep_message->flags & RREP_ACK) != 0){
        //[TODO]
        return LOGGING;
    }

    return LOGGING;
}

uint8_t recv_rerr(uint32_t sender_ip, uint8_t * rerr_message){
    // Seperating out the header and the pairs
    rerr_header * rerr_message_header = (rerr_header *) rerr_message;
    uint32_t *dest_pairs = (uint32_t *) (&rerr_message[sizeof(rerr_header)]);

    /*
        destinations in the RERR
        for which there exists a corresponding entry in the local routing
        table that has the transmitter of the received RERR as the next hop.
    */
    routing_entry * invalid_dest = NULL;
    uint32_t single_rerr = 0;
    uint8_t broadcast_rerr = 0, num_dests = 0;
    uint32_t *dest_ip_seq_pairs = (uint32_t *) malloc(sizeof(uint32_t) * 2 * rerr_message_header->dest_count);
    for(uint8_t i = 0; i < rerr_message_header->dest_count; i++){
        // Get the routing entry for the destination
        invalid_dest = get_routing_entry(routes, dest_pairs[i*2]);
        // If there is an entry and the sender is the next_hop
        if(invalid_dest != NULL && invalid_dest->next_hop == sender_ip && invalid_dest->status == ROUTE_VALID){
            pthread_mutex_lock(&invalid_dest->entry_mutex);
            // Check for precursors
            if(invalid_dest->precursor_list->first != NULL){
                if(invalid_dest->precursor_list->first == invalid_dest->precursor_list->last && single_rerr == 0){
                    single_rerr = invalid_dest->precursor_list->first->data;
                }
                else{
                    broadcast_rerr = 1;
                }
                // Storing the pairs to resend
                dest_ip_seq_pairs[num_dests] = invalid_dest->dest_ip;
                dest_ip_seq_pairs[num_dests + 1] = dest_pairs[(i*2) + 1];
                num_dests += 2;
            }
            /*
            1. The destination sequence number of this routing entry, if it
                exists and is valid, is incremented for cases (i) and (ii) above,
                and copied from the incoming RERR in case (iii) above.

            2. The entry is invalidated by marking the route entry as invalid

            3. The Lifetime field is updated to current time plus DELETE_PERIOD.
                Before this time, the entry SHOULD NOT be deleted.
            */
            invalid_dest->dest_seq = dest_pairs[(i*2) + 1];
            DeleteEntry(invalid_dest->dest_ip, invalid_dest->next_hop);
            invalid_dest->status = ROUTE_INVALID;
            set_expiration_timer(invalid_dest, DELETE_PERIOD);
            if(active_routes > 0){
                active_routes--;
            }

            pthread_mutex_unlock(&invalid_dest->entry_mutex);
        }
    }
    // If it does not unicast or broadcast the rerr
    if(num_dests == 0){
        return LOGGING;
    }
    uint32_t packet_length = 0;
    uint8_t *rerr_buff = generate_rerr_message_buff(&packet_length, 0, num_dests/2, dest_ip_seq_pairs);
    if(single_rerr != 0 && broadcast_rerr == 0){
        SendUnicast(single_rerr, rerr_buff, NULL);
    }  
    else{
        SendBroadcast(rerr_buff, NULL);
    }
    free(dest_ip_seq_pairs);
    free(rerr_buff);
    return LOGGING;
}