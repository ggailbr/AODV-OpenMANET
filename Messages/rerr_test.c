#include "rerr.h"

int main(int argc, char *argv[]){
    int length = 0;
    uint8_t *packet = generate_rerr_message(&length, 1, 2, 0x12345678, 0x87654321, 0x7, 0x10);
    printf("\nRERR Test\n---------------------\n");
    printf("Sizes:\n\tStructure rerr_header: %d bytes\n\tpacket: %d bytes\n", sizeof(rerr_header), length);
    printf("Should have print a packet with the following hex: \n\t0x38000002\n\t0x12345678\n\t0x87654321\n\t0x00000007\n\t0x00000010\n");
    printf("Formatted Packet: \n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n\t0x%08x\n", ((uint32_t *)packet)[0],((uint32_t *)packet)[1],((uint32_t *)packet)[2],((uint32_t *)packet)[3],((uint32_t *)packet)[4]);
    return 0;
}