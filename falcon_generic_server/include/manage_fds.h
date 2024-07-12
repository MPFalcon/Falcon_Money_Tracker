/** @file manage_fds.h
 *
 * @brief Handles polling of sockets
 *
 */

#ifndef MANAGE_FDS_H
#    define MANAGE_FDS_H

#    include "net_io_stream.h"

typedef struct pollfd pollfd_t;

typedef struct func_args
{
    int client_fd;
    void * args;
} args_t;

typedef struct session
{
    pollfd_t * client_poll_fd;
    job_f  associated_job;
    free_f custom_free;
    void * args;
} session_t;

/**
 * @brief             Set the up polling mechanism
 * 
 * @param threadpool  Valid thread pool instance
 * @param const_func  Valid job to assign threads to
 * @param free_func   Valid function that frees associated arguments
 * @param svr_sock    Server FD
 * 
 * @return            SUCCESS: 0
 *                    FAILURE: 1
 */
int setup_poll(threadpool_t * threadpool, job_f const_func, free_f free_func, void * args, int svr_sock);

#endif /* MANAGE_FDS_H */

/*** end of file ***/
