#include "client_session.h"
#include "prog_feats/login.h"
#include "prog_feats/signup.h"

void * session_menu_active(int client, void * args)
{
    meta_data_t meta_data = {
        .bytes_received = 0, .bytes_sent = 0, .msg_len = 0, .msg = { 0 }
    };

    profile_t * user_profile = NULL;
    data_t *    opcode       = NULL;

    if (NULL == args)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    (void)args;

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
            //user_profile = create_profile(instruction_set, meta_data, client);
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
            goto EXIT;
    }

EXIT:

    free(user_profile);
    user_profile = NULL;

    return NULL;
}

/*** end of file ***/
