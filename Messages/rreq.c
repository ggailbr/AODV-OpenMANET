#include "rreq.h"

uint8_t *generate_rreq_message(uint8_t flags, uint32_t rreq_id, uint32_t dest_ip, uint32_t dest_seq, uint32_t origin_ip, uint32_t origin_seq){
    rreq_header * header = (rreq_header *)malloc(sizeof(rreq_header));
    header->type = RREQ_TYPE;
    header->flags = flags;
    header->ttl = 0x0;
    header->hop_count = 0;
    header->rreq_id = rreq_id;
    header->dest_ip = dest_ip;
    header->dest_seq = dest_seq;
    header->src_ip = origin_ip;
    header->src_seq = origin_seq;
    debprintf("[PACKET-RREQ] : 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x\n", ((uint32_t *) header)[0], ((uint32_t *) header)[1], ((uint32_t *) header)[2], ((uint32_t *) header)[3], ((uint32_t *) header)[4], ((uint32_t *) header)[5]);
    return (uint8_t *) header;
}
