#    define _GNU_SOURCE

#    include "../include/server_interface.h"

#    define MAX_PORT_STR_LEN 6

/**
 * @brief               Initiate server
 *
 * @param svr_sock      Server socket
 * @param port          Valid port number to listen on
 *
 * @return              SUCCESS: 0
 *                      FAILURE: 1
 */
static int server_setup(int          * svr_sock,
                        uint16_t       port);

/**
 * @brief               Properly and gracefully shut down server and
 *                      thread pool
 *
 * @param svr_sock      Server socket
 * @param p_threadpool  Valid thread pool instance
 *
 * @return              SUCCESS: 0
 *                      FAILURE: 1
 */
static int server_shutdown(int svr_sock, threadpool_t * p_threadpool);

int setup_driver(config_t * configurations)
{
    int err_code = E_FAILURE;
    int svr_sock = 0;
    threadpool_t * threadpool = NULL;

    if (NULL == configurations)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Something went wrong creating threadpool: %s\n\n",
            __func__);

        goto EXIT;
    }

    threadpool = threadpool_create(configurations->thread_count);

    if (NULL == threadpool)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Something went wrong creating threadpool: %s\n\n",
            __func__);

        goto EXIT;
    }

    err_code = server_setup(&svr_sock, configurations->port);

    if (SUCCESS != err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Something went wrong setting up the server: %s\n\n",
            __func__);
        
        goto EXIT;
    }

    // Manage FDs with poll

    err_code = setup_poll(threadpool, configurations->requested_func, configurations->requested_free_func, configurations->requested_args, configurations->timeout, svr_sock);

    if (SUCCESS != err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Something went wrong setting up the polling: %s\n\n",
            __func__);

        goto EXIT;
    }

    // Shut server down

    err_code = server_shutdown(svr_sock, threadpool);

    if (SUCCESS != err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Something went wrong shuting down server: %s\n\n",
            __func__);

        goto EXIT;
    }

    err_code = SUCCESS;

EXIT:

    return err_code;
}

static int server_setup(int          * svr_sock,
                        uint16_t       port)
{
    int err_code = E_FAILURE;
    int ret_code = 0;

    char port_buffer[MAX_PORT_STR_LEN];

    int opt = 1;

    socklen_t server_addr_len = sizeof(struct sockaddr_in);

    struct addrinfo * results = NULL;
    struct addrinfo * curr_results = NULL;

    struct addrinfo hints = { 0 };
   
    if (NULL == svr_sock)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_IP;

    (void)snprintf(port_buffer, MAX_PORT_STR_LEN, "%hu", port);

    ret_code = getaddrinfo("127.0.0.1", port_buffer, &hints, &results);

    if (SUCCESS != ret_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Failed to receive address information : %s\n\n", __func__);

        goto EXIT;
    }
    
    // Ensure address and socket can be used again
    
    *svr_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (ERROR == *svr_sock)
    {
        DEBUG_PRINT("\n\nERROR [x]  Failed to initiate socket: %s\n\n",
                    __func__);

        goto EXIT;
    }
    
    err_code =
            setsockopt(*svr_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            
    if (SUCCESS != err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Failed to ensure address and port can be reused: %s\n\n",
            __func__);

        goto EXIT;
    }
    
    for (curr_results = results; curr_results != NULL; curr_results = curr_results->ai_next)
    {
        if (NULL != curr_results->ai_canonname)
        {
            printf("canonical: %s\n", curr_results->ai_canonname);
        }

        // Bind sockaddr struct information to file descriptor on system

        err_code =
            bind(*svr_sock, curr_results->ai_addr, server_addr_len);

        if (SUCCESS == err_code)
        {
            break;
        }
    }
    
    if (SUCCESS != err_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Failed to bind socket to address: %s\n\n",
                    __func__);

        goto EXIT;
    }
    
    // Listen on that socket

    err_code = listen(*svr_sock, BACKLOG_CAPACITY);

    if (SUCCESS != err_code)
    {
        DEBUG_PRINT("\n\nERROR [x] Failed to set up listener: %s\n\n",
                    __func__);
    }

EXIT:

    return err_code;
}

static int server_shutdown(int svr_sock, threadpool_t * p_threadpool)
{
    int err_code = E_FAILURE;

    // err_code = shutdown(svr_sock, SHUT_RDWR);

    // if (SUCCESS != err_code)
    // {
    //     DEBUG_PRINT("\n\nERROR [x]  Failed to shut down server: %s\n\n",
    //     __func__);
    // }

    //err_code = notify_clients();

    safe_close(svr_sock);

    err_code = threadpool_shutdown(p_threadpool);

    if (SUCCESS != err_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Failed to shut down thread pool: %s\n\n",
                    __func__);

        goto EXIT;
    }

    err_code = threadpool_destroy(&p_threadpool);

    if (SUCCESS != err_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Failed to destroy thread pool: %s\n\n",
                    __func__);

        goto EXIT;
    }

EXIT:

    return err_code;
}

/*** end of file ***/
