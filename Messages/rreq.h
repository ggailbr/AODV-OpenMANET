#ifndef RREQ_H
#define RREQ_H

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AODV.h"

// The Join Flag for Multicast
#define RREQ_JOIN       0b10000U
// The Repair Flag for Multicast
#define RREQ_REPAIR     0b01000U
// Gratuitous flag to make a gratuitous RREP
#define RREQ_GRAT       0b00100U
// Destination only can respond to RREQ
#define RREQ_DEST_ONLY  0b00010U
// Unknown destination sequence number
#define RREQ_UNKNOWN    0b00001U


/**
 * \struct rreq_message
 * @brief The constant header for a RREQ Message Format
 * \var rreq_header::type
 *  The type of message, should be 1
 * \var rreq_header::flags
 *  A bit field containing the different flags for an RREQ message
 * \var rreq_header::ttl
 *  Repurposing reserved bits as ttl
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
    uint32_t flags : 5;
    uint32_t ttl : 11;
    uint32_t hop_count : 8;
    uint32_t rreq_id;
    uint32_t dest_ip;
    uint32_t dest_seq;
    uint32_t src_ip;
    uint32_t src_seq;

    #elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t hop_count : 8;
    uint32_t ttl : 11;
    uint32_t flags : 5;
    uint32_t type : 8;
    uint32_t rreq_id;
    uint32_t dest_ip;
    uint32_t dest_seq;
    uint32_t src_ip;
    uint32_t src_seq;
    #else
    # error "No Endianness detected"
    #endif
} rreq_header;
/**
 * @brief Generates a fresh RREQ Message. This should only be done by the originator
 *  The RREQ ID MUST be incremented prior to calling this function
 *  The originator sequence number MUST be incremented prior to calling this function
 * 
 * @param flags Binary OR of flags to pass into the function. Set to 0 otherwise
 *  RREQ_JOIN, RREQ_REPAIR, RREQ_GRAT, RREQ_DEST_ONLY, RREQ_UNKNOWN
 * @param rreq_id The incremented RREQ_ID to store
 *  Must be incremented for each attempt
 * @param dest_ip The IP of the destination of the RREQ
 * @param dest_seq The last known Sequence number or blank (0) when RREQ_UNKNOWN
 * @param origin_ip The IP address of this device
 * @param origin_seq The sequence number increments before sending the initial RREQ
 *  Not specified if it should be incremented for subsequent attempts
 * @return A buffer containing the packed members of the packet
 */
uint8_t *generate_rreq_message(uint8_t flags, uint32_t rreq_id, uint32_t dest_ip, uint32_t dest_seq, uint32_t origin_ip, uint32_t origin_seq);

/**
 * @brief Increments the hop count in an existing message
 *  This is the only thing that should be done by intermediate nodes
 * 
 * @param received_packet The buffer of data to increment
 */
void increment_hop_rreq(uint8_t *received_packet);

#endif