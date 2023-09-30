#ifndef RERR_H
#define RERR_H

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AODV.h"


/**
 * \struct rerr_header
 * \brief The constant header for a RERR Message Format
 * \var rerr_header::type
 *  The type of message, should be 3
 * \var rerr_header::n
 *  The no delete flag
 * \var rerr_header::reserved
 *  No use yet, should be sent as 0
 * \var rerr_header::dest_count
 *  The number of unreachable destination, must be >= 1
 */
typedef struct rerr_header_t{
    #if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t type : 8;
    uint32_t n : 1;
    uint32_t reserved : 15;
    uint32_t dest_count : 8;
    #elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t dest_count : 8;
    uint32_t reserved : 15;
    uint32_t n : 1;
    uint32_t type : 8;
    #else
    # error "No Endianness detected"
    #endif
} rerr_header;
/**
 * \brief Creates a RERR message buffer based on input values
 * 
 * \param[out] packet_length The size of the final packet returned
 * \param[in] n The no delete flag
 * \param[in] number_dests The number of invalid destinations passed in
 * \param ... A variable number of pairs of IP and Destination Sequences. All uint32_t
 *      I.E pass in a variable number of destination IP and sequence pairs with each as a uint32_t
 * \return Pointer to the buffer containing the formatted RERR message
 */
uint8_t *generate_rerr_message(int *packet_length, int n, int number_dests, ...);

#endif