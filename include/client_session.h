/** @file client_session.h
 *
 * @brief Central interface for an independent session with client
 *
 */

#ifndef CLIENT_SESSION_H
#    define CLIENT_SESSION_H

#    include "net_io_stream.h"

#    define AUTH_CLIENT 0xfeb4593fecc67839ULL

typedef struct __attribute__((packed)) INSTRUCTION_HEADER
{
    uint16_t op_code;
    uint64_t byte_size;
} instruction_hdr_t;

/**
 * @brief           Welcome new client
 *
 * @param client    Client socket
 */
void session_welcome(int client);

/**
 * @brief                Await instructions from the user
 *
 * @param client         Client socket
 */
bool session_menu_active(int client);

#endif /* CLIENT_SESSION_H */

/*** end of file ***/
