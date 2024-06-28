#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "client_session.h"

#define MAX_MSG_LEN   4098
#define TIMEOUT_MS    1000
#define CLIENT_ACTIVE 1
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

void session_driver(int client)
{
    char name[255];

    // fcntl(client, F_SETFD, O_NONBLOCK);

    meta_data_t   meta_data = { 0 };

    for (;;)
    {
        if (SIGNAL_IDLE != signal_flag_g)
        {
            print_to_client(meta_data, client, "Server Has Shut Down");

            break;
        }
    }

    close(client);

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
