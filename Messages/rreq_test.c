#include "rreq.h"
#include <string.h>

uint32_t dest_ip = 0xC0A80007;
uint32_t dest_seq = 0x00000003;
uint32_t src_ip = 0xC0A80003;
uint32_t src_seq = 0x00000007;

int main(int argc, char *argv[]){
    if(argc >= 3){
        dest_ip = atoi(argv[1]);
        dest_seq = atoi(argv[2]);
        if(argc == 5){
            src_ip = atoi(argv[3]);
            src_seq = atoi(argv[4]);
        }
    }
    uint8_t*  rreq_packet = generate_rreq_message(RREQ_JOIN | RREQ_DEST_ONLY, 0x0U, dest_ip, dest_seq, src_ip, src_seq);
    printf("\nRREQ Test\n---------------------\n");
    printf("Sizes:\n\tStructure rreq_header: %d bytes\n", sizeof(rreq_header));
    printf("Should have print a packet with the following hex: \n\t0x19000000\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n", 0x0U, dest_ip, dest_seq, src_ip, src_seq);
    printf("Formatted Packet: \n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n", ((uint32_t *)rreq_packet)[0],((uint32_t *)rreq_packet)[1],((uint32_t *)rreq_packet)[2],((uint32_t *)rreq_packet)[3],((uint32_t *)rreq_packet)[4],((uint32_t *)rreq_packet)[5]);
    increment_hop_rreq(rreq_packet);
    printf("\nRREQ Increment Test\n---------------------\n");
    printf("Should have print a packet with the following hex: \n\t0x19000001\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n", 0x0U, dest_ip, dest_seq, src_ip, src_seq);
    printf("Formatted Packet: \n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n", ((uint32_t *)rreq_packet)[0],((uint32_t *)rreq_packet)[1],((uint32_t *)rreq_packet)[2],((uint32_t *)rreq_packet)[3],((uint32_t *)rreq_packet)[4],((uint32_t *)rreq_packet)[5]);
    free(rreq_packet);
    return 0;
}