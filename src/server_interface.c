#include "server_interface.h"

#define CLIENT_LIST_NOFULL 0x0
#define CLIENT_LIST_FULL   0x1

#define MAX_CLIENT_LISTS 10
#define CAPACITY         3

typedef struct pollfd pollfd_t;
typedef struct poll_fd_node
{
    uint16_t              position;
    uint8_t               flag;
    uint64_t              active_clients;
    pollfd_t              client_list[CAPACITY];
    struct poll_fd_node * next;
} poll_fd_node_t;
typedef struct poll_fd_list
{
    poll_fd_node_t * head;
    poll_fd_node_t * tail;
} poll_fd_list_t;

/**
 * @brief               Initiate server
 *
 * @param svr_sock      Server socket
 * @param port          Valid port number to listen on
 *
 * @return              SUCCESS: 0
 *                      FAILURE: 1
 */
static int server_setup(int svr_sock, uint16_t port);

/**
 * @brief               Create a new poll section
 *
 * @param poll_list     Valid poll list instance
 * @param server_fd     Server file descriptor
 *
 * @return              SUCCESS: 0
 *                      FAILURE: 1
 */
static int create_new_node(poll_fd_list_t * poll_list, int server_fd);

/**
 * @brief                   Iterate through poll section
 *
 * @param client_list_node  Valid poll section instance
 * @param server_fd         Server file descriptor
 *
 * @return                  SUCCESS: 0
 *                          FAILURE: 1
 */
static int list_iteration(poll_fd_node_t * client_list_node, int server_fd);

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
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in server_setup() : %s\n\n",
                    __func__);

        goto EXIT;
    }

    err_code = server_shutdown(svr_sock);

    if (E_SUCCESS != err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Error occurred in server_shutdown() : %s\n\n",
            __func__);

        goto EXIT;
    }

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static int server_setup(int svr_sock, uint16_t port)
{
    int err_code = E_FAILURE;

    poll_fd_list_t poll_list = { .head = NULL, .tail = NULL };

    struct sockaddr_in server_skt_t;

    int opt = 1;

    socklen_t server_addr_len = sizeof(struct sockaddr_in);

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

    err_code = listen(svr_sock, CAPACITY);

    if (E_SUCCESS != err_code)
    {
        DEBUG_PRINT("\n\nERROR [x] Failed to set up listener: %s\n\n",
                    __func__);

        goto EXIT;
    }

    err_code = create_new_node(&poll_list, svr_sock);

    if (E_SUCCESS != err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Error occurred at create_new_node(): %s\n\n",
            __func__);

        goto EXIT;
    }

    poll_fd_node_t * curr_node = poll_list.head;

    for (;;)
    {
        if (SIGNAL_IDLE != signal_flag_g)
        {
            break;
        }

        err_code = list_iteration(curr_node, svr_sock);

        if (E_SUCCESS != err_code)
        {
            DEBUG_PRINT(
                "\n\nERROR [x]  Error occurred at list_iteration(): %s\n\n",
                __func__);

            goto EXIT;
        }

        if ((0 != (curr_node->flag & CLIENT_LIST_FULL)) &&
            (poll_list.head == curr_node->next) &&
            (MAX_CLIENT_LISTS >= curr_node->position))
        {
            err_code = create_new_node(&poll_list, svr_sock);

            if (E_SUCCESS != err_code)
            {
                DEBUG_PRINT(
                    "\n\nERROR [x]  Error occurred at create_new_node(): "
                    "%s\n\n",
                    __func__);

                goto EXIT;
            }
        }

        curr_node = curr_node->next;
    }

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static int create_new_node(poll_fd_list_t * poll_list, int server_fd)
{
    int err_code = E_FAILURE;

    poll_fd_node_t * new_node = NULL;

    if (NULL == poll_list)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (NULL == poll_list->head)
    {
        new_node = (poll_fd_node_t *)calloc(1, sizeof(poll_fd_node_t));

        if (NULL == new_node)
        {
            DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n",
                        __func__);

            goto EXIT;
        }

        new_node->position       = 0;
        new_node->flag           = CLIENT_LIST_NOFULL;
        new_node->active_clients = 1;
        memset(&new_node->client_list, -1, sizeof(new_node->client_list));
        new_node->client_list[0].fd      = server_fd;
        new_node->client_list[0].events  = POLLIN;
        new_node->client_list[0].revents = 0;

        poll_list->head = new_node;
        poll_list->tail = new_node;

        poll_list->tail->next = poll_list->head;
        err_code              = E_SUCCESS;

        goto EXIT;
    }

    poll_fd_node_t * curr_node = poll_list->head;

    while (poll_list->head != curr_node->next)
    {
        curr_node = curr_node->next;
    }

    new_node = (poll_fd_node_t *)calloc(1, sizeof(poll_fd_node_t));

    if (NULL == new_node)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    new_node->position       = (curr_node->position + 1);
    new_node->flag           = CLIENT_LIST_NOFULL;
    new_node->active_clients = 1;
    memset(&new_node->client_list, -1, sizeof(new_node->client_list));
    new_node->client_list[0].fd      = server_fd,
    new_node->client_list[0].events  = POLLIN,
    new_node->client_list[0].revents = 0;

    curr_node->next = new_node;

    poll_list->tail       = new_node;
    poll_list->tail->next = poll_list->head;

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static int list_iteration(poll_fd_node_t * client_list_node, int server_fd)
{
    int err_code  = E_FAILURE;
    int client_fd = 0;
    int con_exit  = EXIT_FAILURE;

    uint64_t new_client_idx = 0;

    struct sockaddr_in client_skt_t;

    socklen_t client_addr_len = sizeof(struct sockaddr_in);

    if (NULL == client_list_node)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (poll(client_list_node->client_list,
             client_list_node->active_clients,
             -1) == -1)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Error occurred at poll() in section #%hu: %s\n\n",
            client_list_node->position,
            __func__);

        goto EXIT;
    }

    for (uint64_t idx = 0; client_list_node->active_clients > idx; idx++)
    {
        if (0 >= client_list_node->client_list[idx].fd)
        {
            continue;
        }

        if ((client_list_node->client_list[idx].revents & POLLIN) == POLLIN)
        {
            if (server_fd == client_list_node->client_list[idx].fd)
            {
                if (client_list_node->active_clients == CAPACITY)
                {
                    client_list_node->flag =
                        (client_list_node->flag | CLIENT_LIST_FULL);

                    err_code = E_SUCCESS;

                    continue;
                }

                if (ERROR ==
                    (client_fd = accept(server_fd,
                                        (struct sockaddr *)&client_skt_t,
                                        &client_addr_len)))
                {
                    DEBUG_PRINT(
                        "\n\nERROR [x]  Failed to accept client: %s\n\n",
                        __func__);

                    continue;
                }

                client_list_node->active_clients++;
                new_client_idx = (idx + (client_list_node->active_clients - 1));

                client_list_node->client_list[new_client_idx].fd = client_fd;
                client_list_node->client_list[new_client_idx].events  = POLLIN;
                client_list_node->client_list[new_client_idx].revents = 0;

                printf(
                    "\n\nClients active on server #%d in section #%hu: %lu\n\n",
                    server_fd,
                    client_list_node->position,
                    client_list_node->active_clients);

                fcntl(client_list_node->client_list[new_client_idx].fd,
                      F_SETFD,
                      O_NONBLOCK);

                session_driver(client_list_node->client_list[new_client_idx].fd,
                               &con_exit);

                if (EXIT_SUCCESS == con_exit)
                {
                }
            }
            else
            {
                if (POLLOUT == client_list_node->client_list[idx].revents)
                {
                    printf("\n\nClient #%d left\n\n",
                           client_list_node->client_list[idx].fd);
                    close(client_list_node->client_list[idx].fd);
                    client_list_node->client_list[idx].fd *= -1;
                }
            }
            // Add session menu here
        }
    }

    err_code = E_SUCCESS;

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

    // err_code = notify_clients();

    close(svr_sock);

    err_code = E_SUCCESS;

    // EXIT:

    return err_code;
}

/*** end of file ***/
