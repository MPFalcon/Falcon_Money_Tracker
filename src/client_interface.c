#include "client_interface.h"

/**
 * @brief               Locate client based on file descriptor
 *
 * @param client_node   Valid client instance
 * @param curr_node     Valid instance of current node
 *
 * @return              Returned object in memory
 */
static void * locate_client(void * client_node, void * curr_node);

/**
 * @brief           Free client node
 *
 * @param args      Valid client node instance
 */
static void free_client(void * args);

/**
 * @brief           Initiate Client
 *
 * @param client    Client FD
 * @param args      Valid client instance
 *
 * @return          NULL
 */
static void * start_client(int client, void * args);

int setup_session(config_t * server_config)
{
    int err_code = E_FAILURE;

    list_t * active_client_list = NULL;

    if (NULL == server_config)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    active_client_list = list_new((FREE_F)free_client, (CMP_F)locate_client);

    if (NULL == active_client_list)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in list_new() : %s\n\n",
                    __func__);

        goto EXIT;
    }

    server_config->requested_args = (void *)&active_client_list;
    server_config->requested_func = (session_func)start_client;

    err_code = setup_driver(server_config);

    if (E_FAILURE == err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Error occurred in signal_action_setup() : %s",
            __func__);

        goto EXIT;
    }

    err_code = delete_list(&active_client_list);

    if (E_FAILURE == err_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in delete_list() : %s",
                    __func__);

        goto EXIT;
    }

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static void * start_client(int client, void * args)
{
    client_t * new_client = NULL;

    if (NULL == args)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    new_client = (client_t *)calloc(1, sizeof(client_t));

    if (NULL == new_client)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    new_client->client_fd         = client;
    new_client->comp_func         = (CMP_F)locate_client;
    new_client->session_athorized = false;
    new_client->list_refernce     = (list_t **)args;

    list_push_head((*new_client->list_refernce), (void *)new_client);

    session_menu_active((void *)new_client);

EXIT:

    return NULL;
}

static void * locate_client(void * client_node, void * curr_node)
{
    void * ret_client = NULL;

    printf("\n\nRunning\n\n");

    if ((NULL == client_node) || (NULL == curr_node))
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    printf("\n\nList of FDs in memory: %d\n\n",
           ((client_t *)((list_node_t *)curr_node)->data)->client_fd);

    if (((client_t *)client_node)->client_fd ==
        ((client_t *)((list_node_t *)curr_node)->data)->client_fd)
    {
        ret_client = curr_node;
    }

EXIT:

    return ret_client;
}

static void free_client(void * args)
{
    if (NULL != args)
    {
        free(args);
        args = NULL;
    }
}