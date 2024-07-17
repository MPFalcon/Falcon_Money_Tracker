#include "manage_fds.h"

typedef struct poll_manager
{
    threadpool_t * threadpool;
    session_func   const_func;
    free_f         free_func;
    void *         args;
    int            svr_sock;
    pollfd_t *     poll_list;
    nfds_t         idx;
    nfds_t         poll_limit;
    nfds_t         active_connections;
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
static void * client_driver(void * args);

/**
 * @brief           Free memory associated with client_driver()
 */
static void free_client_session(void * data);

/**
 * @brief           Add new connection to pollfd array
 *
 * @param pfds      Valid pollfd array passed in memory
 * @param newfd     New connection (FD)
 * @param fd_count  Numer of active connections
 * @param fd_size   Size of the pollfd array
 */
static void add_to_pfds(pollfd_t ** pfds,
                        int         newfd,
                        nfds_t *    fd_count,
                        nfds_t *    fd_size);

/**
 * @brief           Delete terminated conection from pollfd array
 *
 * @param pfds      Valid pollfd array
 * @param idx       Location of terminated connection
 * @param fd_count  Number of active connections
 */
static void del_from_pfds(pollfd_t * pfds, nfds_t idx, nfds_t * fd_count);

int setup_poll(threadpool_t * threadpool,
               session_func   const_func,
               free_f         free_func,
               void *         args,
               int            timeout,
               int            svr_sock)
{
    int err_code       = E_FAILURE;
    int event_occurred = 0;

    manager_t poll_config = { .threadpool         = threadpool,
                              .const_func         = const_func,
                              .free_func          = free_func,
                              .args               = args,
                              .svr_sock           = svr_sock,
                              .poll_list          = NULL,
                              .idx                = 0,
                              .poll_limit         = BACKLOG_CAPACITY,
                              .active_connections = 1 };

    if (NULL == threadpool)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    poll_config.poll_list =
        (pollfd_t *)calloc(BACKLOG_CAPACITY, sizeof(pollfd_t));

    if (NULL == poll_config.poll_list)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    memset(poll_config.poll_list, -1, (BACKLOG_CAPACITY * sizeof(pollfd_t)));

    poll_config.poll_list[0].fd      = svr_sock;
    poll_config.poll_list[0].events  = POLLIN;
    poll_config.poll_list[0].revents = 0;

    for (;;)
    {
        if (SIGNAL_IGNORED != signal_flag_g)
        {
            break;
        }

        event_occurred =
            poll(poll_config.poll_list, poll_config.poll_limit, timeout);

        if (ERROR == event_occurred)
        {
            DEBUG_PRINT("\n\nERROR [x]  Error occurred in poll() : %s\n\n",
                        __func__);

            goto EXIT;
        }

        for (poll_config.idx = 0; poll_config.poll_limit > poll_config.idx;
             poll_config.idx++)
        {
            if (poll_config.poll_list[poll_config.idx].fd <= STDERR_FILENO)
            {
                continue;
            }

            if (POLLIN ==
                (POLLIN & poll_config.poll_list[poll_config.idx].revents))
            {
                err_code = probe_fd(&poll_config);

                if (E_SUCCESS != err_code)
                {
                    DEBUG_PRINT(
                        "\n\nERROR [x]  Something went wrong calling "
                        "probe_fd() "
                        "with FD #%d : %s\n\n",
                        poll_config.poll_list[poll_config.idx].fd,
                        __func__);
                }
            }
        }
    }

    err_code = E_SUCCESS;

EXIT:

    if (NULL != threadpool)
    {
        pthread_mutex_lock(&threadpool->p_mutex_id);
        threadpool->exit_flag = STOP;
        pthread_mutex_unlock(&threadpool->p_mutex_id);
    }

    if (NULL != poll_config.poll_list)
    {
        free(poll_config.poll_list);
        poll_config.poll_list = NULL;
    }

    return err_code;
}

static int probe_fd(manager_t * poll_config)
{
    int               err_code       = E_FAILURE;
    int               client_fd      = 0;
    int               check_con      = 0;
    struct sockaddr * client_skt     = NULL;
    socklen_t         client_skt_len = 0;
    session_t *       new_session    = NULL;
    char              con_buffer     = 0;

    if (NULL == poll_config)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (poll_config->svr_sock == poll_config->poll_list[poll_config->idx].fd)
    {
        client_fd = accept(poll_config->svr_sock, client_skt, &client_skt_len);

        if (ERROR == client_fd)
        {
            DEBUG_PRINT("\n\nERROR [x]  Error occurred in accept(): %s\n\n",
                        __func__);

            goto EXIT;
        }

        fcntl(client_fd, F_SETFD, O_NONBLOCK);

        add_to_pfds(&poll_config->poll_list,
                    client_fd,
                    &poll_config->active_connections,
                    &poll_config->poll_limit);

        new_session = (session_t *)calloc(1, sizeof(session_t));

        if (NULL == new_session)
        {
            DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n",
                        __func__);

            goto EXIT;
        }

        new_session->associated_job = poll_config->const_func;
        new_session->custom_free    = poll_config->free_func;
        new_session->client_poll_fd =
            &poll_config->poll_list[(poll_config->active_connections - 1)];
        new_session->args = poll_config->args;

        err_code = threadpool_add_job(poll_config->threadpool,
                                      (job_f)client_driver,
                                      (free_f)free_client_session,
                                      (void *)new_session);

        if (SUCCESS != err_code)
        {
            DEBUG_PRINT(
                "\n\nERROR [x]  Error occurred in threadpool_add_job(): %s\n\n",
                __func__);
        }
    }
    else
    {
        check_con = recv(poll_config->poll_list[poll_config->idx].fd,
                         &con_buffer,
                         1,
                         O_NONBLOCK);

        if (0 == check_con)
        {
            safe_close(poll_config->poll_list[poll_config->idx].fd);
            del_from_pfds(poll_config->poll_list,
                          poll_config->idx,
                          &poll_config->active_connections);
        }
    }

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static void * client_driver(void * args)
{
    session_t * session    = NULL;
    args_t *    custom_job = NULL;

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

    session->associated_job(custom_job->client_fd, custom_job->args);
    session->custom_free(custom_job->args);

    session->client_poll_fd->fd *= -1;

    free(custom_job);
    custom_job = NULL;

EXIT:

    return NULL;
}

static void free_client_session(void * data)
{
    if (NULL != data)
    {
        free(data);
        data = NULL;
    }
}

static void add_to_pfds(pollfd_t ** pfds,
                        int         newfd,
                        nfds_t *    fd_count,
                        nfds_t *    fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size)
    {
        *fd_size *= 2; // Double it

        *pfds = (pollfd_t *)realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd     = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
static void del_from_pfds(pollfd_t * pfds, nfds_t idx, nfds_t * fd_count)
{
    // Copy the one from the end over this one
    pfds[idx] = pfds[((*fd_count) - 1)];

    (*fd_count)--;
}

// Main

/*** end of file ***/
