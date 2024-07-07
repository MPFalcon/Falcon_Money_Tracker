#include "client_session.h"
#include "prog_feats/login.h"
#include "prog_feats/signup.h"

bool session_menu_active(int client, struct pollfd * client_poll)
{
    bool session_active = true;

    meta_data_t meta_data = {
        .bytes_received = 0, .bytes_sent = 0, .msg_len = 0, .msg = { 0 }
    };

    profile_t * user_profile = NULL;

    instruction_hdr_t * instruction_set =
        receive_instructions(client, meta_data);

    if (NULL == instruction_set)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    switch (instruction_set->op_code)
    {
        case RECV_READY:

            break;
        case SEND_READY:

            break;
        case LOGIN:

            break;
        case SIGNUP:
            user_profile = create_profile(instruction_set, meta_data, client);

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
            session_active = false;

            break;
        default:
            goto EXIT;
    }

EXIT:

    if (false == session_active)
    {
        close(client_poll->fd);
        client_poll->fd *= -1;
    }

    free(user_profile);
    user_profile = NULL;

    free(instruction_set);
    instruction_set = NULL;

    return session_active;
}

/*** end of file ***/
