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
    data_t *      opcode           = NULL;
    uint64_t      auth_token       = 0ULL;
    char          con_buffer       = 0;
    int           check_con        = 0;
    int           err_code         = E_FAILURE;

    if (NULL == args)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    client_session = (client_t *)args;

    check_con = recv(
        client_session->client_fd, &con_buffer, 1, (O_NONBLOCK | MSG_PEEK));

    if (0 == check_con)
    {
        err_code = list_remove((*client_session->list_refernce),
                               (void **)&client_session);

        if (E_FAILURE == err_code)
        {
            DEBUG_PRINT("\n\nERROR [x]  Error occurred in list_remove() : %s",
                        __func__);
        }

        goto EXIT;
    }

    printf("\n\nNOTE [+]  Session starting for client #%d ... : %s\n\n",
           client_session->client_fd,
           __func__);

    client_list_node = list_find_first_occurrence(
        (*client_session->list_refernce), (void **)&client_session);

    printf("\n\nClient Node in memory: %p\n\n", (void *)client_list_node);

    if (NULL != client_list_node)
    {
        printf("\n\nNeed Authentication\n\n");
        if (false == ((client_t *)client_list_node->data)->session_athorized)
        {

            auth_token =
                ((data_t *)recieve_data(client_session->client_fd, meta_data))
                    ->unsign64;
            (void)convert_endianess64(&auth_token);

            printf("\n\nGiven Token: %lx\n\n", auth_token);

            if (AUTH_CLIENT != auth_token)
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

    // char * balenci = (char *)recieve_data(client, meta_data);

    // printf("\n\nIncoming String: %s\n\n", balenci);

    opcode = (data_t *)recieve_data(client_session->client_fd, meta_data);

    switch (opcode->unsign16)
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

    free(user_profile);
    user_profile = NULL;

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
