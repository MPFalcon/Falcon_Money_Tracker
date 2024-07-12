#include "manage_fds.h"

/**
 * @brief                   Process a single file descriptor
 * 
 * @param threadpool        Valid thread pool instance
 * @param const_func        User-defined function
 * @param free_func         User-defined function to free associated memory
 * @param args              User-defined arguments
 * @param svr_sock          Server FD
 * @param current_pollfd    Valid instance of current pollfd being processed
 *
 * @return                  SUCCESS: 0
 *                          FAILURE: 1 
 */

/**
 * @brief                       Process a single file descriptor
 * 
 * @param threadpool            Valid thread pool instance
 * @param const_func            User-defined function
 * @param free_func             User-defined function to free associated memory
 * @param args                  User-defined arguments
 * @param svr_sock              Server FD
 * @param poll_list             Valid array of pollfd structures
 * @param idx                   Index of current pollfd
 * @param active_connections    Number of active connections
 * 
 * @return                      SUCCESS: 0
 *                              FAILURE: 1 
 */
static int probe_fd(threadpool_t * threadpool, job_f const_func, free_f free_func, void * args, int svr_sock, pollfd_t * poll_list, nfds_t idx, int * active_connections);

/**
 * @brief           Wrapper function around job that is going to be ran
 * 
 * @param args      Valid session
 * 
 * @return          NULL 
 */
static job_f client_driver(void * args);

int setup_poll(threadpool_t * threadpool, job_f const_func, free_f free_func, void * args, int svr_sock)
{
    int err_code = E_FAILURE;
    int active_connections = 0;
    int events_occurred = 0;
    nfds_t poll_limit = 0;

    pollfd_t * poll_list = NULL;

    if (NULL == threadpool)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    poll_list = (pollfd_t *)calloc(BACKLOG_CAPACITY, sizeof(pollfd_t));

    if (NULL == poll_list)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    poll_list[0].fd = svr_sock;
    poll_list[0].events = POLLIN;
    poll_list[0].revents = 0;

    while (SIGNAL_IGNORED == signal_flag_g)
    {
        events_occurred = poll(poll_list, poll_limit, 5);

        for (nfds_t idx = 0; idx <= poll_limit; idx++)
        {
            if (POLLIN == poll_list[idx].revents)
            {
                err_code = probe_fd(threadpool, const_func, free_func, args, svr_sock, poll_list, idx, &active_connections);

                if (E_SUCCESS != err_code)
                {
                    DEBUG_PRINT("\n\nERROR [x]  Something went wrong calling probe_fd() with FD #%d : %s\n\n", poll_list[idx].fd, __func__);
                }
            }
        }
    }

EXIT:

    return err_code;
}

static int probe_fd(threadpool_t * threadpool, job_f const_func, free_f free_func, void * args, int svr_sock, pollfd_t * poll_list, nfds_t idx, int * active_connections)
{
    int err_code = E_FAILURE;
    session_t * new_session = NULL;

    if ((NULL == threadpool) || (NULL == poll_list) || (NULL == active_connections))
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (svr_sock == current_pollfd->fd)
    {
        new_session = (session_t *)calloc(1, sizeof(session_t));

        if (NULL == new_session)
        {
            DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

            goto EXIT;
        }

        poll_list[(active_connections + 1)] = accept(c)
    }

EXIT:

    return err_code;
}

static job_f client_driver(void * args)
{
    session_t * session = NULL;
    args_t * custom_job = NULL;

    if (NULL == args)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    custom_job = (args_t *)calloc(1, sizeof(args_t));

    if (NULL == custom_job)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    session = (session_t *)args;

    custom_job->client_fd = session->client_poll_fd->fd;
    session->client_poll_fd->fd *= -1;
    custom_job->args = session->args;
    session->associated_job(custom_job);
    session->custom_free(custom_job->args);
    session->client_poll_fd->fd *= -1;

EXIT:

    return NULL;
}

/*** end of file ***/
