/** @file client_session.h
 *
 * @brief Central interface for an independent session with client
 *
 */

#ifndef CLIENT_SESSION_H
#    define CLIENT_SESSION_H

#    include "net_io_stream.h"

typedef struct __attribute__((packed)) INSTRUCTION_HEADER
{
    uint16_t op_code;
    uint64_t byte_size;
} instruction_hdr_t;

/**
 * @brief               Spin up session for new client
 *
 * @param thread_count  Valid client instance
 */
void session_driver(int client);

#endif /* CLIENT_SESSION_H */

/*** end of file ***/
