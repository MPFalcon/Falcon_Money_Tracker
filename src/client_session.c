#include "client_session.h"

#define MAX_MSG_LEN   4098
#define TIMEOUT_MS    1000
#define CLIENT_ACTIVE 1

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

void session_welcome(int client)
{
    printf("\n\nClient #%d Connected\n\n", client);

    meta_data_t meta_data = {
        .bytes_received = 0, .bytes_sent = 0, .msg_len = 0, .msg = { 0 }
    };

    meta_data.msg_len = snprintf(
        meta_data.msg, MAX_MSG_LEN, "\n\nHello, Client #%d\n\n", client);

    print_to_client(meta_data, client, meta_data.msg);

    return;
}

void session_welcome(int client)
{
    printf("\n\nClient #%d Connected\n\n", client);

    meta_data_t meta_data = {
        .bytes_received = 0, .bytes_sent = 0, .msg_len = 0, .msg = { 0 }
    };

    meta_data.msg_len = snprintf(
        meta_data.msg, MAX_MSG_LEN, "\n\nHello, Client #%d\n\n", client);

    print_to_client(meta_data, client, meta_data.msg);

    return;
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
