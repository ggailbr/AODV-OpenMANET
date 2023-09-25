#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * \struct rreq_message
 * \brief The constant header for a RREQ Message Format
 * \var rreq_header::type
 *  The type of message, should be 3
 * \var rreq_header::n
 *  No delete flag. Set when a node has performed a local repair of a link
 * \var rreq_header::reserved
 *  No use yet, should be sent as 0
 * \var rreq_header::hop_count
 *  The number of unreachable destination, must be >= 1
 * 
 * \note N may not be in the correct bit position!
 */
typedef struct rreq_header_t{
    uint32_t type : 8;
    uint32_t j : 1;
    uint32_t r : 1;
    uint32_t g : 1;
    uint32_t d : 1;
    uint32_t u : 1;
    uint32_t reserved : 11;
    uint32_t hop_count : 8;
} rreq_header;

uint8_t *generate_rreq_message(int *packet_length, int number_dests, ...);