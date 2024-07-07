/** @file client_session.h
 *
 * @brief Central interface for an independent session with client
 *
 */

#ifndef CLIENT_SESSION_H
#    define CLIENT_SESSION_H

#    include "common/entities.h"

#    define AUTH_CLIENT 0xfeb4593fecc67839ULL

/**
 * @brief                Await instructions from the user
 *
 * @param client         Client socket
 * @param client_poll    Valid client poll instance
 */
bool session_menu_active(int client, struct pollfd * client_poll);

#endif /* CLIENT_SESSION_H */

/*** end of file ***/
