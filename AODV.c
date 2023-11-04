
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
void hello_interval();

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
    return 0;
}

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