#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "client_session.h"

#define MAX_MSG_LEN     4098

typedef struct net_meta_data
{
    ssize_t bytes_received;
    ssize_t bytes_sent;
    ssize_t msg_len;
    char    msg[MAX_MSG_LEN];
} meta_data_t;

void session_driver(int client)
{
    return;
}

/*** end of file ***/
