#include "rerr.h"

uint8_t *generate_rerr_message(int *packet_length, int n, int number_dests, ...){
    rerr_header header;
    if(number_dests <= 0){
        // Should be >=1 !!!!
        return NULL;
    }
    // Setting Header type and number of destionations
    // IGNORING N for now
    header.type = 3;
    header.n = n;
    header.reserved = 0;
    // header.reserved_b = 0x8000;
    // header.reserved_b &= (n<<15);
    header.dest_count = number_dests;
    printf("%d", sizeof(header));
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
    return packet;
}