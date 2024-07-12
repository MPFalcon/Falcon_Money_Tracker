/** @file server_interface.h
 *
 * @brief Central interface for the main server
 *
 */

#ifndef SERVER_INTERFACE_H
#    define SERVER_INTERFACE_H

#    include "manage_fds.h"

/**
 * @brief               
 *
 * @param port          

 */

/**
 * @brief               Set the up driver function to start up server
 * 
 * @param thread_count  Number of threads to spin up
 * @param port          Requested port number to operate on
 * @param const_func    User-defined function
 * @param free_func     User-defined function to free associated memory
 * @param args          User-defined arguments
 *
 * @return              SUCCESS: 0
 *                      FAILURE: 1 
 */
int setup_driver(uint32_t thread_count, uint16_t port, job_f const_func, free_f free_func, void * args);

#endif /* SERVER_INTERFACE_H */

/*** end of file ***/
