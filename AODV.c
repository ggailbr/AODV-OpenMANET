#include "MANET-Testbed/api.h"
#include <stdio.h>

/*
 * This is a protocol main function. This is run once at the start 
 * of route creation. 
*/
int main(int argc, char **argv){
    InitializeApi();
    printf("Test\n");
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