#include "server_interface.h"

#define CLIENT_LIST_NOFULL 0x0
#define CLIENT_LIST_FULL   0x1

#define MAX_CLIENT_LISTS 10
#define CAPACITY         5
#define TIMEOUT_MS       1

typedef struct pollfd pollfd_t;
typedef struct poll_fd_node
{
    uint16_t              position;
    uint8_t               flag;
    uint64_t              active_clients;
    uint64_t              enqueue_point;
    uint64_t              next_available_spot;
    uint64_t              available_spots[(CAPACITY - 1)];
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
 * @brief               Initiate server
 *
 * @param curr_node     Valid node iterator instance
 * @param svr_sock      Server FD
 * @param poll_list     Valid poll list instance
 *
 * @return              SUCCESS: 0
 *                      FAILURE: 1
 */
static int initiate_server(poll_fd_node_t * curr_node,
                           int              svr_sock,
                           poll_fd_list_t * poll_list);

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
 * @brief       Determine if newly established session is authorized
 *
 * @param fd    Client FD
 *
 * @return      SUCCESS: 0
 *              FAILURE: 1
 */
static int connection_authorized(int fd);

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
 * @brief                       Process specific file descriptor in poller
 *
 * @param server_fd             Server FD
 * @param client_list_node      Valid client list instance
 * @param client_skt_t          Valid client FD instance
 * @param client_addr_len       Valid client address length instance
 * @param idx                   Position of the FD in the poller
 *
 * @return                      SUCCESS: 0
 *                              FAILURE: 1
 */
static int process_fd(int                  server_fd,
                      poll_fd_node_t *     client_list_node,
                      struct sockaddr_in * client_skt_t,
                      socklen_t *          client_addr_len,
                      uint64_t             idx);

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

/**
 * @brief               Free memory associate with main poller list
 *
 * @param poll_list     Valid poller list instance
 * @param prev_node     Valid pointer to trailing poller
 * @param curr_node     Valid pointer to current poller
 */
static void free_poll_list(poll_fd_list_t *  poll_list,
                           poll_fd_node_t ** prev_node,
                           poll_fd_node_t ** curr_node);

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

    poll_fd_list_t   poll_list = { .head = NULL, .tail = NULL };
    poll_fd_node_t * curr_node = NULL;
    poll_fd_node_t * prev_node = NULL;

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

    fcntl(svr_sock, F_SETFD, O_NONBLOCK);

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

    curr_node = poll_list.head;

    err_code = initiate_server(curr_node, svr_sock, &poll_list);

    if (E_SUCCESS != err_code)
    {
        goto EXIT;
    }

    err_code = E_SUCCESS;

EXIT:

    free_poll_list(&poll_list, &prev_node, &curr_node);

    return err_code;
}

static int initiate_server(poll_fd_node_t * curr_node,
                           int              svr_sock,
                           poll_fd_list_t * poll_list)
{
    int err_code = E_FAILURE;

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
            (poll_list->head == curr_node->next) &&
            (MAX_CLIENT_LISTS >= curr_node->position))
        {
            err_code = create_new_node(poll_list, svr_sock);

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

        new_node->position            = 0;
        new_node->flag                = CLIENT_LIST_NOFULL;
        new_node->active_clients      = 1;
        new_node->enqueue_point       = 1;
        new_node->next_available_spot = 0;
        memset(&new_node->client_list, -1, (CAPACITY * sizeof(pollfd_t)));
        new_node->client_list[0].fd      = server_fd;
        new_node->client_list[0].events  = POLLIN;
        new_node->client_list[0].revents = 0;
        new_node->available_spots[0]     = 1;

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

    new_node->position            = (curr_node->position + 1);
    new_node->flag                = CLIENT_LIST_NOFULL;
    new_node->active_clients      = 1;
    new_node->enqueue_point       = 1;
    new_node->next_available_spot = 0;
    memset(&new_node->client_list, -1, (CAPACITY * sizeof(pollfd_t)));
    new_node->client_list[0].fd      = server_fd,
    new_node->client_list[0].events  = POLLIN,
    new_node->client_list[0].revents = 0;
    new_node->available_spots[0]     = 1;

    curr_node->next = new_node;

    poll_list->tail       = new_node;
    poll_list->tail->next = poll_list->head;

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static int connection_authorized(int fd)
{
    int      err_code            = E_FAILURE;
    uint64_t authorization_token = 0;

    meta_data_t meta_data = {
        .bytes_received = 0, .bytes_sent = 0, .msg_len = 0, .msg = { 0 }
    };

    meta_data.bytes_received =
        recv(fd, &authorization_token, sizeof(uint64_t), O_NONBLOCK);

    if (ERROR == meta_data.bytes_received)
    {
        goto EXIT;
    }

    (void)convert_endianess64(&authorization_token);

    if (AUTH_CLIENT != authorization_token)
    {
        goto EXIT;
    }

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static int list_iteration(poll_fd_node_t * client_list_node, int server_fd)
{
    int err_code = E_FAILURE;

    struct sockaddr_in client_skt_t;

    socklen_t client_addr_len = sizeof(struct sockaddr_in);

    if (NULL == client_list_node)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (poll(client_list_node->client_list, CAPACITY, TIMEOUT_MS) == -1)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Error occurred at poll() in section #%hu: %s\n\n",
            client_list_node->position,
            __func__);

        goto EXIT;
    }

    for (uint64_t idx = 0; CAPACITY > idx; idx++)
    {
        if (0 >= client_list_node->client_list[idx].fd)
        {
            client_list_node->available_spots[client_list_node->enqueue_point] =
                idx;
            client_list_node->enqueue_point =
                ((client_list_node->enqueue_point + 1) % (CAPACITY - 1));

            continue;
        }

        if ((client_list_node->client_list[idx].revents & POLLIN) == POLLIN)
        {
            err_code = process_fd(server_fd,
                                  client_list_node,
                                  &client_skt_t,
                                  &client_addr_len,
                                  idx);

            if (E_FAILURE == err_code)
            {
                break;
            }
        }
    }

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static int process_fd(int                  server_fd,
                      poll_fd_node_t *     client_list_node,
                      struct sockaddr_in * client_skt_t,
                      socklen_t *          client_addr_len,
                      uint64_t             idx)
{
    int err_code       = E_SUCCESS;
    int client_fd      = 0;
    int new_client_idx = 0;

    if ((NULL == client_list_node) || (NULL == client_skt_t) ||
        (NULL == client_addr_len))
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        err_code = E_FAILURE;

        goto EXIT;
    }

    if (server_fd == client_list_node->client_list[idx].fd)
    {
        if (client_list_node->active_clients == CAPACITY)
        {
            client_list_node->flag =
                (client_list_node->flag | CLIENT_LIST_FULL);

            goto EXIT;
        }

        if (ERROR == (client_fd = accept(server_fd,
                                         (struct sockaddr *)client_skt_t,
                                         client_addr_len)))
        {
            DEBUG_PRINT("\n\nERROR [x]  Failed to accept client: %s\n\n",
                        __func__);

            goto EXIT;
        }

        if (E_FAILURE == connection_authorized(client_fd))
        {
            close(client_fd);

            goto EXIT;
        }

        new_client_idx =
            client_list_node
                ->available_spots[client_list_node->next_available_spot];
        client_list_node->next_available_spot =
            ((1 + client_list_node->next_available_spot) % (CAPACITY - 1));

        client_list_node->client_list[new_client_idx].fd      = client_fd;
        client_list_node->client_list[new_client_idx].events  = POLLIN;
        client_list_node->client_list[new_client_idx].revents = 0;

        fcntl(client_list_node->client_list[new_client_idx].fd,
              F_SETFD,
              O_NONBLOCK);

        client_list_node->active_clients++;

        // printf("\n\nClients active on server #%d in section #%hu: %lu\n\n",
        //        server_fd,
        //        client_list_node->position,
        //        client_list_node->active_clients);
        // if (false ==
        // session_menu_active(client_list_node->client_list[idx].fd,
        //                                  &client_list_node->client_list[idx]))
        // {
        //     client_list_node->active_clients--;
        // }
    }
    else if (false == session_menu_active(client_list_node->client_list[idx].fd,
                                          &client_list_node->client_list[idx]))
    {
        client_list_node->active_clients--;
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

    // err_code = notify_clients();

    close(svr_sock);

    err_code = E_SUCCESS;

    // EXIT:

    return err_code;
}

static void free_poll_list(poll_fd_list_t *  poll_list,
                           poll_fd_node_t ** prev_node,
                           poll_fd_node_t ** curr_node)
{
    if ((NULL == poll_list) || (NULL == prev_node) || (NULL == curr_node))
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    *prev_node = poll_list->head;
    *curr_node = poll_list->head;

    while (poll_list->head != (*curr_node)->next)
    {
        *prev_node = *curr_node;
        *curr_node = (*curr_node)->next;

        free(*prev_node);
    }

    free(*curr_node);

    *prev_node      = NULL;
    *curr_node      = NULL;
    poll_list->head = NULL;
    poll_list->tail = NULL;

EXIT:

    return;
}

/*** end of file ***/
