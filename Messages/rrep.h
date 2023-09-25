#ifndef RREP_H
#define RREP_H

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../AODV.h"

/**
 * \struct rrep_message
 * \brief The constant header for a rrep Message Format
 * \var rrep_header::type
 *  The type of message, should be 2
 * \var rrep_header::r
 *  Repair flag
 * \var rrep_header::a
 *  Acknowledgement required
 * \var rrep_header::reserved
 *  No use yet, should be sent as 0
 * \var rrep_header::prefix_size
 *  If !=0, specifies that the next hope may be used for any nodes with
 *  same routing prefix as the requested destination
 * \var rrep_header::hop_count
 *  Number of hops from originator IP address to node handling the request
 * \var rrep_header::dest_ip
 *  The IP address of destination for which route is supplied
 * \var rrep_header::dest_seq
 *  The destination sequence number associated to the route
 * \var rrep_header::src_ip
 *  The IP address of the node which originated the Route Request
 * \var rrep_header::lifetime
 *  The time in ms for which the nodes receiving the RREP consider the 
 *  route valid.
 * 
 * \note N may not be in the correct bit position!
 */
typedef struct rrep_header_t{
    #if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t type : 8;
    uint32_t r : 1;
    uint32_t a : 1;
    uint32_t reserved : 9;
    uint32_t prefix_size : 5;
    uint32_t hop_count : 8;
    uint32_t dest_ip;
    uint32_t dest_seq;
    uint32_t src_ip;
    uint32_t lifetime;

    #elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t lifetime;
    uint32_t src_ip;
    uint32_t dest_seq;
    uint32_t dest_ip;
    uint32_t hop_count : 8;
    uint32_t prefix_size : 5;
    uint32_t reserved : 9;
    uint32_t a : 1;
    uint32_t r : 1;
    uint32_t type : 8;
    #else
    # error "No Endianness detected"
    #endif
} rrep_header;

#endif