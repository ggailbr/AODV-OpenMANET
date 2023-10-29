#include "send_messages.h"
#include "time_funcs.h"


/**
 * @brief The routine for sending an rrep from the destionation. Sends
 *  a RREP back to the sender of the rreq
 * 
 * @param message The rreq message received
 * @param sender_ip The IP of the sender
 */
void send_rrep_destination(rreq_header *message, uint32_t sender_ip){
    // Incrementing seq number and setting to max to received and incremented
    /*
    6.1) -  Immediately before a destination node originates a RREP in
      response to a RREQ, it MUST update its own sequence number to the
      maximum of its current sequence number and the destination
      sequence number in the RREQ packet.
    */
    /*
     6.6.1) If the generating node is the destination itself, it MUST increment
    its own sequence number by one if the sequence number in the RREQ
    packet is equal to that incremented value.  Otherwise, the
    destination does not change its sequence number before generating the
    RREP message.
   */
    /*
        I am interpreting this as, increment own sequence number, set to
        maximum of supplied and incremented
    */
    uint32_t seq = read_safe(&sequence_num);
    seq++;
    // If the sequence number is known
    if(message->flags & RREQ_UNKNOWN == 0){
        seq = seq_compare(message->dest_seq, seq) < 0 ? seq : message->dest_seq;
    }
    write_safe(&sequence_num, seq);
    // Copying information and sending
    uint8_t *rrep_buf = generate_rrep_message(0, 0, ip_address, seq, message->src_ip, MY_ROUTE_TIMEOUT);
    // Send it back along sender's path
    SendUnicast(sender_ip, rrep_buf, NULL);
    // Free the message buffer made
    free(rrep_buf);
}

/**
 * @brief Routine for sending a RREP from an intemediate node.
 *  Sends back rrep to sender and forwards to destination if needed
 * 
 * @param message The RREQ message received
 * @param sender The forwarder of the rreq
 */
void send_rrep_intermediate(rreq_header *message, uint32_t sender){
    // Getting "fresh enough" route entry
    routing_entry *dest_entry = get_routing_entry(routes, message->dest_ip);
    // Lock entry for reading
    pthread_mutex_lock(&dest_entry->entry_mutex);
    // Get the time remaining in the timeout for destination
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    subtract_time(&current_time, &dest_entry->time_out);
    // Add sender to precursor list of forward route
    add_entry_to_list(&dest_entry->precursor_list, sender);
    // Unlock Entry
    pthread_mutex_unlock(&dest_entry->entry_mutex);

    // Create rrep message
    uint8_t *rrep_buf = generate_rrep_message(0, 0, message->dest_ip, dest_entry->dest_seq, message->src_ip, convert_timespec_to_ms(&current_time));
    // Moving the Hop Count
    ((rrep_header *) rrep_buf)->hop_count =  dest_entry->hop_count;
    // Sending RREP to Sender
    SendUnicast(sender, rrep_buf, NULL);
    free(rrep_buf);

    // Add next hop to precursor of reverse route
    routing_entry * origin_entry = get_routing_entry(routes, message->src_ip);
    pthread_mutex_lock(&origin_entry->entry_mutex);
    add_entry_to_list(&origin_entry->precursor_list,  dest_entry->next_hop);
    pthread_mutex_unlock(&origin_entry->entry_mutex);

    // Check for gratuitous messages
    if(message->flags && RREQ_GRAT){
        // Setting Creating reverse RREP
        uint8_t *rrep_buf = generate_rrep_message(0, 0, message->src_ip, message->src_seq, message->dest_ip, ACTIVE_ROUTE_TIMEOUT);
        // Change the hop count
        ((rrep_header *) rrep_buf)->hop_count = message->hop_count;
        SendUnicast(dest_entry->next_hop, rrep_buf, NULL);
        free(rrep_buf);
    }
}

/**
 * @brief The thread function to send the rreq message
 *  This will continually send rreq's until a rrep
 *  results in the termination of the thread
 * 
 * @param dest_entry The destination entry to send the rreq for
 */
void send_rreq(routing_entry * dest_entry){
    uint32_t ttl = TTL_START;
    while(ttl <= TTL_)
    // Incrementing id and sequence number
    uint32_t id = increment_safe(&rreq_id);
    uint32_t seq = increment_safe(&sequence_num);
    // Setting all to be gratuitous by default
    uint8_t flags = RREQ_GRAT;
    // Setting the sequence number for the destination
    uint32_t dest_seq = 0;
    if(dest_entry == NULL || dest_entry->seq_valid != SEQ_VALID){
        flags |= RREQ_UNKNOWN;
    }
    else{
        dest_seq = dest_entry->dest_seq;
    }
    uint8_t *rreq_message = generate_rreq_message(flags, id, dest_ip, dest_seq, src_ip, seq);
    // [TODO]
    // set_ttl();
    SendBroadcast(rreq_message, NULL);
}

