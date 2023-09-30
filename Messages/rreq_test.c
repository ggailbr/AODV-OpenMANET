#include "rreq.h"
#include <string.h>

uint32_t dest_ip = 0xC0A80007;
uint32_t dest_seq = 0x00000003;
uint32_t src_ip = 0xC0A80003;
uint32_t src_seq = 0x00000007;

void main(int argc, char *argv[]){
    if(argc >= 3){
        dest_ip = atoi(argv[1]);
        dest_seq = atoi(argv[2]);
        if(argc == 5){
            src_ip = atoi(argv[3]);
            src_seq = atoi(argv[4]);
        }
    }

}