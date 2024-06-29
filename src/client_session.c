#include "client_session.h"

#define MAX_MSG_LEN  4098
#define MAX_BANK_LEN 200
#define MAX_NAME_LEN 50
#define MAX_PASS_LEN 100

#define AUTH_CLIENT       0xfeb4593fecc67839ULL
#define TERMINATE_SESSION 0xffff

#define CLIENT_ACTIVE 1
typedef struct associated_bank
{
    char      bank_name[MAX_BANK_LEN];
    u_int64_t balance;
} bank_t;
typedef struct profile
{
    uint64_t profile_id;
    char     username[MAX_NAME_LEN];
    char     password[MAX_PASS_LEN];
    bank_t * banks;
} profile_t;
typedef enum instruction_codes
{
    RECV_READY     = 0x12df,
    SEND_READY     = 0x12ab,
    LOGIN          = 0xccdf,
    ADD_BANK       = 0xdadf,
    ADD_BALANCE    = 0x2394,
    REMOVE_BANK    = 0xcc91,
    REMOVE_BALANCE = 0xff43,
    UPDATE_BANK    = 0x6582,
    UPDATE_BALANCE = 0x2239
} instruction_codes_t;
typedef struct net_meta_data
{
    ssize_t bytes_received;
    ssize_t bytes_sent;
    ssize_t msg_len;
    char    msg[MAX_MSG_LEN];
} meta_data_t;

/**
 * @brief           Print to client
 *
 * @param meta_data Metadata structure
 * @param client    Client FD
 * @param p_msg     Message
 */
static void print_to_client(meta_data_t  meta_data,
                            int          client,
                            const char * p_msg);

static instruction_hdr_t * receive_instructions(int         client,
                                                meta_data_t meta_data);

static int send_instructions(int                 client,
                             meta_data_t         meta_data,
                             instruction_hdr_t * instructions);

void session_welcome(int client)
{
    meta_data_t meta_data = {
        .bytes_received = 0, .bytes_sent = 0, .msg_len = 0, .msg = { 0 }
    };

    meta_data.msg_len = snprintf(
        meta_data.msg, MAX_MSG_LEN, "\n\nHello, Client #%d\n\n", client);

    print_to_client(meta_data, client, meta_data.msg);

    return;
}

bool session_menu_active(int client)
{
    // if (NULL == event_occurred)
    // {
    //     goto EXIT;
    // }
    bool     session_active      = true;
    uint64_t authorization_token = 0;

    meta_data_t meta_data = {
        .bytes_received = 0, .bytes_sent = 0, .msg_len = 0, .msg = { 0 }
    };

    meta_data.bytes_received =
        receive_bytes(client, &authorization_token, sizeof(uint64_t));

    if ((ERROR == meta_data.bytes_received) ||
        (AUTH_CLIENT != authorization_token))
    {
        goto EXIT;
    }

    instruction_hdr_t * instruction_set =
        receive_instructions(client, meta_data);

    if (NULL == instruction_set)
    {
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
        default:
            goto EXIT;
    }

    if (instruction_set->op_code == TERMINATE_SESSION)
    {
        session_active = false;
    }

EXIT:

    return session_active;
}

static instruction_hdr_t * receive_instructions(int         client,
                                                meta_data_t meta_data)
{
    instruction_hdr_t * new_instructions = NULL;

    meta_data.bytes_received =
        receive_bytes(client, new_instructions, sizeof(instruction_hdr_t));

    if (ERROR == meta_data.bytes_received)
    {
        goto EXIT;
    }

EXIT:

    return new_instructions;
}

static void print_to_client(meta_data_t  meta_data,
                            int          client,
                            const char * p_msg)
{
    if (NULL == p_msg)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    meta_data.msg_len = snprintf(meta_data.msg, MAX_MSG_LEN, "%s", p_msg);

    // int err_code = send_instructions(p_session, meta_data, MSG_CODE,
    // meta_data.msg_len);

    // if (SUCCESS != err_code)
    // {
    //     DEBUG_PRINT("\n\nERROR [x]  Error occured in send_instructions():
    //     %s\n\n", __func__);

    //     goto EXIT;
    // }

    meta_data.bytes_sent = send_bytes(client, meta_data.msg, meta_data.msg_len);

    if (ERROR == meta_data.bytes_sent)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in send_bytes(): %s\n\n",
                    __func__);
    }

EXIT:

    return;
}

/*** end of file ***/
