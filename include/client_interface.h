/** @file client_interface.h
 *
 * @brief Central interface for an independent session with client
 *
 */

#ifndef CLIENT_INTERFACE_H
#    define CLIENT_INTERFACE_H

#    include "server_interface.h"
#    include "client_session.h"

#    define AUTH_CLIENT 0xfeb4593fecc67839ULL

/**
 * @brief                Await instructions from the user
 *
 * @param client         Client socket
 * @param args           Valid arguments instance
 */
int setup_session();

#endif /* CLIENT_INTERFACE_H */

/*** end of file ***/
