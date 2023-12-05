
#include <stdint.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include "AODV.h"
#include "rreq.h"
#include "send_messages.h"
#include "time_funcs.h"
#include "manet_testbed.h"
#include "rrep.h"


void *send_rreq_thread(void *thread_entry);

/**
 * @brief The routine for sending an rrep from the destionation. Sends
 *  a RREP back to the sender of the rreq
 * 
 * @param message The rreq message received
 * @param sender_ip The IP of the sender
 */
void send_rrep_destination(rreq_header *message, uint32_t sender_ip){
    // debprintf("[RREP] Sending Destination\n");
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
    if((message->flags & RREQ_UNKNOWN) == 0){
        seq = seq_compare(message->dest_seq, seq) < 0 ? seq : message->dest_seq;
    }
    write_safe(&sequence_num, seq);
    // Copying information and sending
    uint8_t *rrep_buf = generate_rrep_message(0, 0, ip_address, seq, message->src_ip, MY_ROUTE_TIMEOUT);
    // Send it back along sender's path
    SendUnicast(sender_ip, rrep_buf, sizeof(rrep_header), NULL);
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
    // debprintf("[RREP] Sending Intermediate\n");
    // Getting "fresh enough" route entry
    routing_entry *dest_entry = get_routing_entry(routes, message->dest_ip);
    // Lock entry for reading
    pthread_mutex_lock(&dest_entry->entry_mutex);
    // Get the time remaining in the timeout for destination
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    subtract_time(&current_time, &dest_entry->time_out);
    // Add sender to precursor list of forward route
    add_entry_to_list(dest_entry->precursor_list, sender);
    // Unlock Entry
    pthread_mutex_unlock(&dest_entry->entry_mutex);

    // Create rrep message
    uint8_t *rrep_buf = generate_rrep_message(0, 0, message->dest_ip, dest_entry->dest_seq, message->src_ip, convert_timespec_to_ms(&current_time));
    // Moving the Hop Count
    ((rrep_header *) rrep_buf)->hop_count =  dest_entry->hop_count;
    // Sending RREP to Sender
    SendUnicast(sender, rrep_buf, sizeof(rreq_header), NULL);
    free(rrep_buf);

    // Add next hop to precursor of reverse route
    routing_entry * origin_entry = get_routing_entry(routes, message->src_ip);
    pthread_mutex_lock(&origin_entry->entry_mutex);
    add_entry_to_list(origin_entry->precursor_list,  dest_entry->next_hop);
    pthread_mutex_unlock(&origin_entry->entry_mutex);

    // Check for gratuitous messages
    if(message->flags && RREQ_GRAT){
        // Setting Creating reverse RREP
        uint8_t *rrep_buf = generate_rrep_message(0, 0, message->src_ip, message->src_seq, message->dest_ip, ACTIVE_ROUTE_TIMEOUT);
        // Change the hop count
        ((rrep_header *) rrep_buf)->hop_count = message->hop_count;
        SendUnicast(dest_entry->next_hop, rrep_buf, sizeof(rreq_header), NULL);
        free(rrep_buf);
    }
}

/**
 * @brief Sends the rreq requires from the originator
 *  Sets up the inital variables before starting a timing thread
 * 
 * @param dest_addr The address to send the message to
 * @return If a route has been found (1) or it failed (0)
 */
uint8_t send_rreq(uint32_t dest_addr){
    uint8_t new;
    char timing_buff[80];
    clock_t starting_time = clock();
    rreq_status outcome = SEARCH_NONE;
    debprintf("[RREQ] Getting destination entry\n");
    routing_entry * dest_entry = create_or_get_routing_entry(routes, dest_addr, 0, SEQ_INVALID, 0, 0, MY_ROUTE_TIMEOUT, &new);
    // If we recently failed, drop the packet
    if(dest_entry->rreq_search == SEARCH_FAILED && dest_entry->status == ROUTE_INVALID){
        return PACKET_DROP;
    }
    // Make sure not already searching and it not a new thread
    if(new == 0 && dest_entry->rreq_search != SEARCH_SEARCHING){
        debprintf("[RREQ] Was not Searching for a route, but entry exists\n");
        if(!(dest_entry->expiration_thread == 0)){
            pthread_cancel(dest_entry->expiration_thread);
            // printf("Canceling %ld\n", dest_entry->expiration_thread);
        }
        dest_entry->expiration_thread = 0;
    }
    // Make sure we are not searching for route already
    if(dest_entry->rreq_search != SEARCH_SEARCHING){
        
        debprintf("[RREQ] Creating a Route Request Thread\n");
        // Locking entry and starting rreq thread
        pthread_mutex_lock(&dest_entry->entry_mutex);
        // Start the search
        pthread_create(&dest_entry->rreq_message_sender, NULL, send_rreq_thread, (void *) dest_entry);
        // Set the status to searching
        dest_entry->rreq_search = SEARCH_SEARCHING;
        pthread_mutex_unlock(&dest_entry->entry_mutex);
    }
    // Wait until we are no longer searching
    // [TODO] Could coded with better threading practices
    
    debprintf("[RREQ] Waiting until the searching status changes\n");
    while(dest_entry->rreq_search == SEARCH_SEARCHING){
        sched_yield();
    }
    // Check the status 
    pthread_mutex_lock(&dest_entry->entry_mutex);
    outcome = dest_entry->rreq_search;
    pthread_mutex_unlock(&dest_entry->entry_mutex);
    // Attempting self Unicast
    sprintf(timing_buff, "[RREQ] %d, %ld, %ld", outcome, clock()-starting_time, CLOCKS_PER_SEC);
    SendUnicast(ip_address, (uint8_t *)timing_buff, sizeof(timing_buff), NULL);

    // If it was a failure
    if(outcome == SEARCH_FAILED){
        debprintf("[RREQ] Search Failed, dropping packet\n");
        pthread_mutex_lock(&dest_entry->entry_mutex);
        set_expiration_timer(dest_entry, 500);
        pthread_mutex_unlock(&dest_entry->entry_mutex);
        // Drop Packet
        return PACKET_DROP;
    }
    else{
        // Accept Packets
        debprintf("[RREQ] Search Succeded, sending packet\n");
        dest_entry->rreq_search = SEARCH_NONE;
        return PACKET_ACCEPT;
    }

}


/**
 * @brief The thread function to send the rreq message
 *  This will continually send rreq's until a rrep
 *  results in the termination of the thread
 * 
 * @param dest_entry The destination entry to send the rreq for
 */
void *send_rreq_thread(void *thread_entry){
    // debprintf("[RREQ_T] Starting Thread %ld\n", ((routing_entry *) thread_entry)->rreq_message_sender);
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    routing_entry * dest_entry = (routing_entry *) thread_entry;
    // Assuming the sequence number should only be incremented
    //  at start of attempting to send
    uint32_t seq = increment_safe(&sequence_num);

    // Grab Destination mutex and info
    pthread_mutex_lock(&dest_entry->entry_mutex);
    uint8_t flags = RREQ_GRAT;
    // Grabbing known hop_count if present
    uint32_t hop_count = dest_entry->hop_count;
    // Grabbing the destination sequence number (if known)
    uint32_t dest_seq = 0;
    if(dest_entry->seq_valid == SEQ_INVALID){
        flags |= RREQ_UNKNOWN;
    }
    else{
        dest_seq = dest_entry->dest_seq;
    }
    // Generate base message
    uint8_t * rreq_buff = generate_rreq_message(flags, seq, dest_entry->dest_ip, dest_seq, ip_address, seq);
    // Ensure memory is freed
    pthread_cleanup_push(free, rreq_buff);
    pthread_mutex_unlock(&dest_entry->entry_mutex);

    // Cast message to header type for indexing
    rreq_header * rreq_message = (rreq_header * )rreq_buff;
    struct timespec wait_time;

    uint32_t ttl = TTL_START>hop_count? TTL_START : hop_count + TTL_INCREMENT;
    // Starting at either TTL_start of hop_count + TTL_INCREMENT
    // Goes until it hits the threshold

    for(; ttl <= TTL_THRESHOLD; ttl += TTL_INCREMENT){
        // Incrementing id
        rreq_message->rreq_id = increment_safe(&rreq_id);
        rreq_message->ttl = ttl;
        SendBroadcast(rreq_buff, sizeof(rreq_header), NULL);
        debprintf("Sending RREQ %d\n", ttl);
        convert_ms_to_timespec(&wait_time, RING_TRAVERSAL_TIME(ttl));
        debprintf("Waiting: %dms\n", convert_timespec_to_ms(&wait_time));
        while(nanosleep(&wait_time, &wait_time));
    }
    ttl = NET_DIAMETER;
    uint32_t fall_off = 1;
    for(uint32_t i = 0; i <= RREQ_RETRIES; i++){
        rreq_message->rreq_id = increment_safe(&rreq_id);
        rreq_message->ttl = ttl;
        SendBroadcast(rreq_buff, sizeof(rreq_header), NULL);
        debprintf("Sending RREQ Max\n");
        convert_ms_to_timespec(&wait_time, NET_TRAVERSAL_TIME * (fall_off << i));
        debprintf("Waiting: %dms\n", convert_timespec_to_ms(&wait_time));
        while(nanosleep(&wait_time, &wait_time));
    }
    // Start expiration thread on failure
    debprintf("[RREQ_T] Sending RREQ FAILED\n");
    pthread_mutex_lock(&dest_entry->entry_mutex);
    debprintf("[RREQ_T] Changed Status\n");
    dest_entry->rreq_message_sender = 0;
    dest_entry->rreq_search = SEARCH_FAILED;
    set_expiration_timer(dest_entry, 2 * NET_TRAVERSAL_TIME);
    pthread_mutex_unlock(&dest_entry->entry_mutex);

    // exiting to call cleanup functions
    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}
