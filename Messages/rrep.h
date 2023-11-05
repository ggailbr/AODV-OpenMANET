#ifndef __RREP_H__
#define __RREP_H__

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AODV.h"

#define RREP_REPAIR 0b10U
#define RREP_ACK 0b1U

/**
 * \struct rrep_message
 * \brief The constant header for a rrep Message Format
 * \var rrep_header::type
 *  The type of message, should be 2
 * \var rrep_header::flags
 *      If an acknowledge or repair flag
 *      Use RREP_ACK and RREP_REPAIR
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
    uint32_t flags : 2;
    uint32_t reserved : 9;
    uint32_t prefix_size : 5;
    uint32_t hop_count : 8;
    uint32_t dest_ip;
    uint32_t dest_seq;
    uint32_t src_ip;
    uint32_t lifetime;

    #elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t hop_count : 8;
    uint32_t prefix_size : 5;
    uint32_t reserved : 9;
    uint32_t flags : 2;
    uint32_t type : 8;
    uint32_t dest_ip;
    uint32_t dest_seq;
    uint32_t src_ip;
    uint32_t lifetime;
    #else
    # error "No Endianness detected"
    #endif
} rrep_header;

/**
 * \brief Generates an RREP message based off of of the given parameters.
 * 
 * @param flags Determines special behavior of the packet
 *      Logical OR of any of the following:
 *          RREP_REPAIR - Multicast repair
 *          RREP_ACK - Acknoledgment required
 * @param prefix_size Used for multicast and not implemented yet
 * @param dest_ip The IP address of the destination for the corresponding RREQ
 * @param dest_seq The Sequence number of this destination
 *      Should be the newest sequence number:
 *          Either the max of the RREQ and current number
 * @param origin_ip The originator of the corresponding RREQ
 * @param time_out The timeout field of the rrep
 * @return 
 */
uint8_t * generate_rrep_message( uint8_t flags, uint8_t prefix_size, uint32_t dest_ip, uint32_t dest_seq, uint32_t origin_ip, uint32_t life_time);

void increment_hop_rrep(uint8_t *received_packet);

#endif