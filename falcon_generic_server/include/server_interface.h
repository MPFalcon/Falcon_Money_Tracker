/** @file server_interface.h
 *
 * @brief Central interface for the main server
 *
 */

#ifndef SERVER_INTERFACE_H
#    define SERVER_INTERFACE_H

#    include "manage_fds.h"

typedef struct server_configuration
{
    uint16_t     port;
    uint32_t     thread_count;
    int          timeout;
    session_func requested_func;
    free_f       requested_free_func;
    void *       requested_args;
} config_t;

/**
 * @brief                Set the up driver function to start up server
 *
 * @param configurations Valid instance of configuration settings
 *
 * @return               SUCCESS: 0
 *                       FAILURE: 1
 */
int setup_driver(config_t * configurations);

#endif /* SERVER_INTERFACE_H */

/*** end of file ***/
