#include "rrep.h"

uint8_t * generate_rrep_message( uint8_t flags, uint8_t prefix_size, uint32_t dest_ip, uint32_t dest_seq, uint32_t origin_ip, uint32_t life_time){
    rrep_header * header = (rrep_header *)malloc(sizeof(rrep_header));
    header->type = RREP_TYPE;
    header->flags = flags;
    header->reserved = 0x0;
    header->hop_count = 0;
    header->dest_ip = dest_ip;
    header->dest_seq = dest_seq;
    header->src_ip = origin_ip;
    header->lifetime = life_time;
    debprintf("[PACKET-RREP] : 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x", ((uint32_t *) header)[0], ((uint32_t *) header)[1], ((uint32_t *) header)[2], ((uint32_t *) header)[3], ((uint32_t *) header)[4]);
    return (uint8_t *) header;
}

void increment_hop_rrep(uint8_t *received_packet){
    ((rrep_header *)received_packet)->hop_count++;
}