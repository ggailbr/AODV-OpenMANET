#include "api.h"
#include <stdio.h>
#include "AODV.h"

// [CHANGE] Manually setting for now, should be changed to the dynamic interface IP
uint32_t ip_address = 0xC4A80007;
safe_32 rreq_id;
safe_32 sequence_num;
/*
 * This is a protocol main function. This is run once at the start 
 * of route creation. 
*/
int main(int argc, char **argv){
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
int incoming_message(){

    return 0;
}