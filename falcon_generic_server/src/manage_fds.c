#include "manage_fds.h"

typedef struct poll_config
{
    threadpool_t * threadpool;
    job_f const_func;
    free_f free_func;
    void * args;
    int svr_sock;
    pollfd_t * poll_list;
    nfds_t * idx;
    nfds_t * poll_limit;
    int * active_connections;
} manager_t;

/**
 * @brief                       Process a single file descriptor
 * 
 * @param poll_config           Valid instance of management data
 * 
 * @return                      SUCCESS: 0
 *                              FAILURE: 1 
 */
static int probe_fd(manager_t * sock_config);

/**
 * @brief           Wrapper function around job that is going to be ran
 * 
 * @param args      Valid session
 * 
 * @return          NULL 
 */
static job_f client_driver(void * args);

/**
 * @brief           Free memory associated with client_driver()
 */
static free_f free_client_session(void * data);

int setup_poll(threadpool_t * threadpool, job_f const_func, free_f free_func, void * args, int timeout, int svr_sock)
{
    int err_code = E_FAILURE;

    if (NULL == threadpool)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    manager_t * 

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
        events_occurred = poll(poll_list, poll_limit, timeout);

        for (nfds_t idx = 0; idx <= poll_limit; idx++)
        {
            if (POLLIN == poll_list[idx].revents)
            {
                err_code = probe_fd(threadpool, const_func, free_func, args, svr_sock, poll_list, idx, poll_limit, &active_connections);

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

static int probe_fd(threadpool_t * threadpool, job_f const_func, free_f free_func, void * args, int svr_sock, pollfd_t * poll_list, nfds_t idx, nfds_t poll_limit, int * active_connections)
{
    int err_code = E_FAILURE;

    struct sockaddr client_skt = { 0 };
    socklen_t   client_skt_len = 0;
    session_t * new_session = NULL;

    if ((NULL == threadpool) || (NULL == poll_list) || (NULL == active_connections))
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (svr_sock == poll_list[idx].fd)
    {
        new_session = (session_t *)calloc(1, sizeof(session_t));

        if (NULL == new_session)
        {
            DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

            goto EXIT;
        }

        poll_list[((*active_connections) + 1)].fd = accept(svr_sock, &client_skt, &client_skt_len);
        poll_list[((*active_connections) + 1)].events = POLLIN;
        poll_list[((*active_connections) + 1)].events = 0;
    }
    else
    {
        new_session = (session_t *)calloc(1, sizeof(session_t));

        if (NULL == new_session)
        {
            DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

            goto EXIT;
        }

        threadpool_add_job(threadpool, client_driver, free_client_session, new_session);
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

    free(session);
    session = NULL;

EXIT:

    return NULL;
}

static free_f free_client_session(void * data)
{
    if (NULL == data)
    {
        goto EXIT;
    }

    args_t * custom_job = (args_t *)data;

    free(custom_job);
    custom_job = NULL;

EXIT:

    return;
}

static void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, nfds_t *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

// Main

/*** end of file ***/
