#ifndef RREQ_H
#define RREQ_H

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../AODV.h"


/**
 * \struct rreq_message
 * \brief The constant header for a RREQ Message Format
 * \var rreq_header::type
 *  The type of message, should be 1
 * \var rreq_header::j
 *  Join flag
 * \var rreq_header::r
 *  Repair flag
 * \var rreq_header::g
 *  Gratuitous RREP flag. If a gratuitous RREP should be unicast to dest_ip
 * \var rreq_header::d
 *  Destination only flag. If only the destination should reply
 * \var rreq_header::u
 *  Unknown sequence number. Dest_seq is unknown
 * \var rreq_header::reserved
 *  No use yet, should be sent as 0
 * \var rreq_header::hop_count
 *  Number of hops from originator IP address to node handling the request
 * \var rreq_header::rreq_id
 *  A sequence number uniquely identifying the particular RREQ
 *  when taken in conjunction with the originating node's IP address.
 * \var rreq_header::dest_ip
 *  The IP address of destination for which route is desired
 * \var rreq_header::dest_seq
 *  The latest sequence number received in past by originator
 * \var rreq_header::src_ip
 *  The IP address of the node which originated the Route Request
 * \var rreq_header::src_seq
 *  The sequence number to be used in route entry pointing towards
 *  the originator of the route request.
 * 
 * \note N may not be in the correct bit position!
 */
typedef struct rreq_header_t{
    #if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t type : 8;
    uint32_t j : 1;
    uint32_t r : 1;
    uint32_t g : 1;
    uint32_t d : 1;
    uint32_t u : 1;
    uint32_t reserved : 11;
    uint32_t hop_count : 8;
    uint32_t rreq_id;
    uint32_t dest_ip;
    uint32_t dest_seq;
    uint32_t src_ip;
    uint32_t src_seq;

    #elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t src_seq;
    uint32_t src_ip;
    uint32_t dest_seq;
    uint32_t dest_ip;
    uint32_t rreq_id;
    uint32_t hop_count : 8;
    uint32_t reserved : 11;
    uint32_t u : 1;
    uint32_t d : 1;
    uint32_t g : 1;
    uint32_t r : 1;
    uint32_t j : 1;
    uint32_t type : 8;
    #else
    # error "No Endianness detected"
    #endif
} rreq_header;
// [TODO]
uint8_t *generate_rreq_message(int *packet_length, int number_dests);

#endif