#include "client_session.h"
#include "prog_feats/login.h"
#include "prog_feats/signup.h"

static int operations(client_t * client_ref, meta_data_t meta_data, uint16_t op_code);

static int authenticate_session(data_t * integral_dat, client_t * client_node_ref, meta_data_t meta_data);

void session_menu_active(void * args)
{
    meta_data_t meta_data = {
        .bytes_received = 0, .bytes_sent = 0, .msg_len = 0, .msg = { 0 }
    };

    list_node_t * client_list_node   = NULL;
    client_t *    client_ref         = NULL;
    client_t *    client_node_ref    = NULL;
    client_t **   client_session_ref = NULL;
    profile_t *   user_profile       = NULL;
    data_t *      integral_dat       = NULL;
    int           err_code           = E_FAILURE;

    if (NULL == args)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    client_session_ref = (client_t **)args;
    client_ref         = (*client_session_ref);

    printf("\n\nNOTE [+]  Session starting for client #%d ... : %s\n\n",
           client_ref->client_fd,
           __func__);

    client_list_node = list_find_first_occurrence((*client_ref->list_refernce),
                                                  (void **)&client_session_ref);

    if (NULL != client_list_node)
    {
        client_node_ref = (*((client_t **)client_list_node->data));
        if (false == client_node_ref->session_athorized)
        {
            err_code = authenticate_session(integral_dat, client_node_ref, meta_data);

            if (E_SUCCESS != err_code)
            {
                DEBUG_PRINT("\n\nERROR [x]  Error occurred in authenticate_session() : %s\n\n", __func__);

                goto EXIT;
            }
        }
    }

    integral_dat = (data_t *)recieve_data(client_ref->client_fd, meta_data);

    if (NULL == integral_dat)
    {
        goto EXIT;
    }

    (void)convert_endianess16(&integral_dat->unsign16);

    err_code = operations(client_ref, meta_data, integral_dat->unsign16);

    if (E_SUCCESS != err_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in operations() : %s\n\n", __func__);

        goto EXIT;
    }

    err_code = E_SUCCESS;

EXIT:

    if (NULL != integral_dat)
    {
        free(integral_dat);
        integral_dat = NULL;
    }

    if (NULL != user_profile)
    {
        free(user_profile);
        user_profile = NULL;
    }

    return;
}

void free_session(void * args)
{
    // if (NULL != args)
    // {
    //     free(args);
    //     args = NULL;
    // }

    (void)args;
}

static int operations(client_t * client_ref, meta_data_t meta_data, uint16_t op_code)
{
    int err_code = E_FAILURE;

    if (NULL == client_ref)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    switch (op_code)
    {
        case RECV_READY:

            break;
        case SEND_READY:

            break;
        case LOGIN:

            break;
        case SIGNUP:
            printf("\nRunning\n");
            (void)send_data(client_ref->client_fd,
                            meta_data,
                            "\n\nWelcome to Falcon Server\n\n",
                            29);
            break;
        case ADD_BANK:

            break;
        case ADD_BALANCE:

            break;
        case REMOVE_BANK:

            break;
        case REMOVE_BALANCE:

            break;
        case UPDATE_BANK:

            break;
        case UPDATE_BALANCE:

            break;
        case TERMINATE_SESSION:

            break;
        default:
            break;
    }

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static int authenticate_session(data_t * integral_dat, client_t * client_node_ref, meta_data_t meta_data)
{
    int err_code = E_FAILURE;

    if ((NULL == integral_dat) || (NULL == client_node_ref))
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    integral_dat =
        ((data_t *)recieve_data(client_node_ref->client_fd, meta_data));

    if (NULL == integral_dat)
    {
        goto EXIT;
    }

    (void)convert_endianess64(&integral_dat->unsign64);

    printf("\n\nGiven Token: %lx\n\n", integral_dat->unsign64);

    if (AUTH_CLIENT != integral_dat->unsign64)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Unauthorized client detected -"
            "Terminating Connection... : %s\n\n",
            __func__);

        goto EXIT;
    }
    else
    {
        client_node_ref->session_athorized = true;

        printf("\n\nNOTE [+]  Client #%d Accepted ... : %s\n\n",
                client_node_ref->client_fd,
                __func__);
    }

    err_code = E_SUCCESS;

EXIT:

    if (NULL != integral_dat)
    {
        free(integral_dat);
        integral_dat = NULL;
    }

    return err_code;
}

/*** end of file ***/
