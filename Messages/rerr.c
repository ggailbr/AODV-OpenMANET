#include "rerr.h"
#include "debug.h"

uint8_t *generate_rerr_message(uint32_t *packet_length, uint8_t n, uint8_t number_dests, ...){
    rerr_header header;
    if(number_dests <= 0){
        // Should be >=1 !!!!
        return NULL;
    }
    // Setting Header type and number of destionations
    // IGNORING N for now
    header.type = RERR_TYPE;
    header.n = n;
    header.reserved = 0;
    // header.reserved_b = 0x8000;
    // header.reserved_b &= (n<<15);
    header.dest_count = number_dests;
    // Reading in the destinations and allocating memory
    va_list dests;
    va_start(dests, number_dests);
    uint8_t *packet = (uint8_t *) malloc(sizeof(rerr_header) + sizeof(uint32_t) * number_dests * 2);
    uint32_t *destination_list = (uint32_t *)(packet + sizeof(rerr_header));
    // Adding the destination and sequence number after the header
    for(int i = 0; i < number_dests; i++){
        destination_list[(2*i)] = va_arg(dests, uint32_t);
        destination_list[(2*i)+1] = va_arg(dests, uint32_t);
    }
    memcpy(packet, &header, sizeof(rerr_header));
    *packet_length = sizeof(rerr_header) + sizeof(uint32_t) * number_dests * 2;
    #ifdef DEBUG
    debprintf("[PACKET-RERR] : 0x%08x", ((uint32_t *) packet)[0]);
    for(int i = 0; i < number_dests; i++){
        debprintf(", 0x%08x, 0x%08x",((uint32_t *) packet)[(2*i)+1], ((uint32_t *) packet)[(2*i)+2]);
    }
    #endif
    return packet;
}
uint8_t *generate_rerr_message_buff(uint32_t *packet_length, uint8_t n, uint8_t number_dests, uint32_t * dest_ip_seq_pairs){
    rerr_header header;
    if(number_dests <= 0){
        // Should be >=1 !!!!
        return NULL;
    }
    // Setting Header type and number of destionations
    // IGNORING N for now
    header.type = RERR_TYPE;
    header.n = n;
    header.reserved = 0;
    header.dest_count = number_dests;

    uint8_t *packet = (uint8_t *) malloc(sizeof(rerr_header) + sizeof(uint32_t) * number_dests * 2);
    uint32_t *destination_list = (uint32_t *)(packet + sizeof(rerr_header));
    memcpy(packet, &header, sizeof(rerr_header));
    memcpy(destination_list, dest_ip_seq_pairs, sizeof(uint32_t) * 2 * number_dests);
    *packet_length = sizeof(rerr_header) + sizeof(uint32_t) * number_dests * 2;
    #ifdef DEBUG
    debprintf("[PACKET-RERR] : 0x%08x", ((uint32_t *) packet)[0]);
    for(int i = 0; i < number_dests; i++){
        debprintf(", 0x%08x, 0x%08x",((uint32_t *) packet)[(2*i)+1], ((uint32_t *) packet)[(2*i)+2]);
    }
    #endif
    return packet;
}