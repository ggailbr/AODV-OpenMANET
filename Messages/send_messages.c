#include "send_messages.h"

void send_rrep_destination(rreq_header *message){
    // Incrementing seq number and setting to max to received and incremented
    uint32_t seq = read_safe(&sequence_num);
    seq = max(seq, message->dest_seq);
    write_safe(&sequence_num, seq);
    // Copying information and sending
    uint8_t *rrep_buf = generate_rrep_message(0, 0, ip_address, seq, message->src_ip);
    SendUnicast(message->src_ip, rrep_buf, NULL);
    free(rrep_buf);
}
void send_rrep_intermediate(rreq_header *message, uint32_t sender){
    // Getting "fresh enough" route entry
    routing_entry *dest_entry = get_routing_entry(routes, message->dest_ip);
    uint8_t *rrep_buf = generate_rrep_message(0, 0, message->dest_ip, dest_entry->dest_seq, message->src_ip);
    // Add sender to precursor list
    add_entry_to_list(&dest_entry->precursor_list, sender);

    // Setting expiration
    rrep_header * rrep_message = (rrep_header *)rrep_buf;
    // Should actually be replaced with expiration time-current_time
    rrep_message->lifetime = dest_entry->time_out;
    // Sending RREP to Sender
    SendUnicast(message->src_ip, rrep_buf, NULL);

    // Check for gratuitous messages
    if(message->flags && RREQ_GRAT){
        free(rrep_buf);
        // Setting Creating reverse RREP
        uint8_t *rrep_buf = generate_rrep_message(0, 0, message->src_ip, message->src_seq, message->dest_ip);
        // Change the hop count
        rrep_header * rrep_message = (rrep_header *)rrep_buf;
        rrep_message->hop_count = message->hop_count;
        SendUnicast(message->dest_ip, rrep_buf, NULL);
        free(rrep_buf);
    }
}

void send_rreq(uint32_t src_ip, uint32_t dest_ip, uint32_t ttl){
    routing_entry *dest_entry = get_routing_entry(routes, dest_ip);
    // Incrementing id and sequence number
    uint32_t id = increment_safe(&rreq_id);
    uint32_t seq = increment_safe(&sequence_num);
    // Adding to rreq id list to prevent retransmission
    add_rreq_entry(rreq_ids, src_ip, id);
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