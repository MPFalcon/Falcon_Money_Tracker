/** @file server_interface.h
 *
 * @brief Central interface for the main server
 *
 */

#ifndef SERVER_INTERFACE_H
#    define SERVER_INTERFACE_H

#    include "client_session.h"

/**
 * @brief               Set the up driver function to start up server
 *
 * @param port          Requested port number to operate on
 *
 * @return              SUCCESS: 0
 *                      FAILURE: 1
 */
int setup_driver(uint16_t port);

#endif /* SERVER_INTERFACE_H */

/*** end of file ***/
