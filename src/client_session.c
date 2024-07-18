#include "client_session.h"
#include "prog_feats/login.h"
#include "prog_feats/signup.h"

void session_menu_active(void * args)
{
    meta_data_t meta_data = {
        .bytes_received = 0, .bytes_sent = 0, .msg_len = 0, .msg = { 0 }
    };

    list_node_t * client_list_node = NULL;
    client_t *    client_session   = NULL;
    profile_t *   user_profile     = NULL;
    data_t *      integral_dat     = NULL;
    int           err_code         = E_FAILURE;

    if (NULL == args)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    client_session = (client_t *)args;

    printf("\n\nNOTE [+]  Session starting for client #%d ... : %s\n\n",
           client_session->client_fd,
           __func__);

    client_list_node = list_find_first_occurrence(
        (*client_session->list_refernce), (void **)&client_session);

    if (NULL != client_list_node)
    {

        if (false == ((client_t *)client_list_node->data)->session_athorized)
        {
            integral_dat =
                ((data_t *)recieve_data(client_session->client_fd, meta_data));

            if (NULL == integral_dat)
            {
                goto EXIT;
            }

            (void)convert_endianess64(&integral_dat->unsign64);

            // printf("\n\nGiven Token: %lx\n\n", integral_dat->unsign64);

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
                ((client_t *)client_list_node->data)->session_athorized = true;

                printf("\n\nNOTE [+]  Client #%d Accepted ... : %s\n\n",
                       client_session->client_fd,
                       __func__);
            }
        }
    }

    if (NULL != integral_dat)
    {
        free(integral_dat);
        integral_dat = NULL;
    }

    integral_dat = (data_t *)recieve_data(client_session->client_fd, meta_data);

    if (NULL == integral_dat)
    {
        goto EXIT;
    }

    (void)convert_endianess16(&integral_dat->unsign16);

    switch (integral_dat->unsign16)
    {
        case RECV_READY:

            break;
        case SEND_READY:

            break;
        case LOGIN:

            break;
        case SIGNUP:
            // user_profile = create_profile(instruction_set, meta_data,
            // client);
            printf("\nRunning\n");
            (void)send_data(client_session->client_fd,
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

    // check_con = recv(
    //     client_session->client_fd, &con_buffer, 1, (O_NONBLOCK | MSG_PEEK));

    // if (0 == check_con)
    // {
    //     err_code = list_remove((*client_session->list_refernce),
    //                            (void **)&client_session);

    //     if (E_FAILURE == err_code)
    //     {
    //         DEBUG_PRINT("\n\nERROR [x]  Error occurred in list_remove() :
    //         %s",
    //                     __func__);
    //     }

    //     safe_close(client_session->client_fd);

    //     goto EXIT;
    // }

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

/*** end of file ***/
