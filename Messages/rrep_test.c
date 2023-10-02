#include "rrep.h"
#include <string.h>

uint32_t dest_ip = 0xC0A80007;
uint32_t dest_seq = 0x00000003;
uint32_t src_ip = 0xC0A80003;

int main(int argc, char *argv[]){
    if(argc >= 3){
        dest_ip = atoi(argv[1]);
        dest_seq = atoi(argv[2]);
        if(argc == 5){
            src_ip = atoi(argv[3]);
        }
    }
    uint8_t*  rrep_packet = generate_rrep_message(RREP_ACK, 0x0U, dest_ip, dest_seq, src_ip);
    printf("\nRREP Test\n---------------------\n");
    printf("Sizes:\n\tStructure rrep_header: %d bytes\n", sizeof(rrep_header));
    printf("Should have print a packet with the following hex: \n\t0x26000000\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n", dest_ip, dest_seq, src_ip, MY_ROUTE_TIMEOUT);
    printf("Formatted Packet: \n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n", ((uint32_t *)rrep_packet)[0],((uint32_t *)rrep_packet)[1],((uint32_t *)rrep_packet)[2],((uint32_t *)rrep_packet)[3],((uint32_t *)rrep_packet)[4]);
    increment_hop_rrep(rrep_packet);
    printf("\nRREP Increment Test\n---------------------\n");
    printf("Should have print a packet with the following hex: \n\t0x26000001\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n", dest_ip, dest_seq, src_ip, MY_ROUTE_TIMEOUT);
    printf("Formatted Packet: \n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n", ((uint32_t *)rrep_packet)[0],((uint32_t *)rrep_packet)[1],((uint32_t *)rrep_packet)[2],((uint32_t *)rrep_packet)[3],((uint32_t *)rrep_packet)[4]);
    free(rrep_packet);
    return 0;
}