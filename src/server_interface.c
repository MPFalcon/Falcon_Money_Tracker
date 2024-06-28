#include "server_interface.h"

#define BACKLOG_CAPACITY 20

/**
 * @brief               Initiate server
 *
 * @param svr_sock      Server socket
 * @param port          Valid port number to listen on
 *
 * @return              SUCCESS: 0
 *                      FAILURE: 1
 */
static int server_setup(int            svr_sock,
                        uint16_t       port);

/**
 * @brief               Properly and gracefully shut down server and
 *                      thread pool
 *
 * @param svr_sock      Server socket
 *
 * @return              SUCCESS: 0
 *                      FAILURE: 1
 */
static int server_shutdown(int svr_sock);

int setup_driver(uint16_t port)
{
    int err_code = E_FAILURE;
    int svr_sock = 0;

    err_code = server_setup(svr_sock, port);

    if (E_SUCCESS != err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Error occurred in server_setup() : %s\n\n",
            __func__);
    }

    err_code = server_shutdown(svr_sock);

     if (E_SUCCESS != err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Error occurred in server_shutdown() : %s\n\n",
            __func__);
    }

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static int server_setup(int            svr_sock,
                        uint16_t       port)
{
    int err_code = E_FAILURE;

    struct sockaddr_in server_skt_t;
    struct sockaddr_in client_skt_t;

    int clnt_skt = 0;
    int opt      = 1;

    socklen_t server_addr_len = sizeof(struct sockaddr_in);
    socklen_t client_addr_len = sizeof(struct sockaddr_in);

    svr_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (ERROR == svr_sock)
    {
        DEBUG_PRINT("\n\nERROR [x]  Failed to initiate socket: %s\n\n",
                    __func__);

        goto EXIT;
    }

    server_skt_t.sin_family      = AF_INET;
    server_skt_t.sin_addr.s_addr = INADDR_ANY;
    server_skt_t.sin_port        = htons(port);

    // Ensure address and socket can be used again

    err_code =
        setsockopt(svr_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (E_SUCCESS != err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Failed to address and port can be reused: %s\n\n",
            __func__);
    }

    // Bind sockaddr struct information to file descriptor on system

    err_code =
        bind(svr_sock, (const struct sockaddr *)&server_skt_t, server_addr_len);

    if (E_SUCCESS != err_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Failed to bind socket to address: %s\n\n",
                    __func__);

        goto EXIT;
    }

    // Listen on that socket

    err_code = listen(svr_sock, BACKLOG_CAPACITY);

    if (E_SUCCESS != err_code)
    {
        DEBUG_PRINT("\n\nERROR [x] Failed to set up listener: %s\n\n",
                    __func__);

        goto EXIT;
    }

    // Accept oncoming connections

    while ((ERROR != (clnt_skt = accept(svr_sock,
                                      (struct sockaddr *)&client_skt_t,
                                      &client_addr_len))))
    {
        if (SIGNAL_IDLE != signal_flag_g)
        {
            break;
        }
        
    }

EXIT:

    return err_code;
}

static int server_shutdown(int svr_sock)
{
    int err_code = E_FAILURE;

    // err_code = shutdown(svr_sock, SHUT_RDWR);

    // if (E_SUCCESS != err_code)
    // {
    //     DEBUG_PRINT("\n\nERROR [x]  Failed to shut down server: %s\n\n",
    //     __func__);
    // }

    //err_code = notify_clients();

    close(svr_sock);

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

/*** end of file ***/
