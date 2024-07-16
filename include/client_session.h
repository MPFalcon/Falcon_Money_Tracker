/** @file client_session.h
 *
 * @brief Central interface for an independent session with client
 *
 */

#ifndef CLIENT_SESSION_H
#    define CLIENT_SESSION_H

#    include "common/entities.h"
#    include "common/packet_manager.h"

#    define AUTH_CLIENT 0xfeb4593fecc67839ULL

/**
 * @brief                Await instructions from the user
 *
 * @param client         Client socket
 * @param args           Valid arguments instance
 */
void * session_menu_active(int client, void * args);

/**
 * @brief       Free client session
 * 
 * @param args  Memory that needs to be freed
 */
void free_session(void * args);

#endif /* CLIENT_SESSION_H */

/*** end of file ***/
