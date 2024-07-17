#include "client_session.h"
#include "prog_feats/login.h"
#include "prog_feats/signup.h"

void * session_menu_active(int client, void * args)
{
    printf("\n\nNOTE [+]  Session starting for client #%d ... : %s\n\n",
           client,
           __func__);

    meta_data_t meta_data = {
        .bytes_received = 0, .bytes_sent = 0, .msg_len = 0, .msg = { 0 }
    };

    profile_t * user_profile = NULL;
    data_t *    opcode       = NULL;
    char        con_buffer   = 0;
    int         check_con    = 0;

    if (NULL == args)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    (void)args;

    if (AUTH_CLIENT != ((data_t *)recieve_data(client, meta_data))->unsign128)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Unauthorized client detected -"
            "Terminating Connection... : %s\n\n",
            __func__);

        goto EXIT;
    }
    else
    {
        printf(
            "\n\nNOTE [+]  Client #%d Accepted ... : %s\n\n", client, __func__);
    }

    for(;;)
    {
        check_con = recv(poll_config->poll_list[poll_config->idx].fd,
                         &con_buffer,
                         1,
                         (O_NONBLOCK));

        if (0 == check_con)
        {
            break;
        }

        opcode = (data_t *)recieve_data(client, meta_data);

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
                continue;
        }
    }

EXIT:

    free(user_profile);
    user_profile = NULL;

    return NULL;
}

void free_session(void * args)
{
    if (NULL != args)
    {
        free(args);
        args = NULL;
    }
}

/*** end of file ***/
