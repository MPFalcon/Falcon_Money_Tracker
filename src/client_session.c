#include "client_session.h"

#define MAX_BANK_LEN 200
#define MAX_NAME_LEN 50
#define MAX_PASS_LEN 100
typedef struct associated_bank
{
    char     bank_name[MAX_BANK_LEN];
    uint64_t balance;
} bank_t;
typedef struct profile
{
    uint64_t profile_id;
    char     username[MAX_NAME_LEN];
    char     password[MAX_PASS_LEN];
    uint32_t bank_count;
    bank_t * banks;
} profile_t;
typedef enum instruction_codes
{
    RECV_READY        = 0xa2df,
    SEND_READY        = 0xa2ab,
    LOGIN             = 0xacdf,
    SIGNUP            = 0xacdc,
    ADD_BANK          = 0xaadf,
    ADD_BALANCE       = 0xa394,
    REMOVE_BANK       = 0xac91,
    REMOVE_BALANCE    = 0xaf43,
    UPDATE_BANK       = 0xa582,
    UPDATE_BALANCE    = 0xa239,
    TERMINATE_SESSION = 0xaffe,
    ERR_CODE          = 0xabfd
} instruction_codes_t;

typedef enum return_code
{
    OP_SUCCESS  = 0xb2df,
    OP_ERR      = 0xb2ab,
    OP_MSGINVAL = 0xbcdf,
    OP_NOTFOUND = 0xbcdc,
    OP_UNKNOWN  = 0xbadf
} ret_code_t;

/**
 * @brief                       Recieve instruction code from client
 *                              for the server to intepret
 *
 * @param client                Client FD
 * @param meta_data             Metadata structure
 *
 * @return                      Instruction set instance
 */
static instruction_hdr_t * receive_instructions(int         client,
                                                meta_data_t meta_data);

/**
 * @brief               Create a profile and add to database
 * 
 * @param instructions  Valid instructions instant
 * @param meta_data     Metadata structure
 * @param client        Client FD
 */
void create_profile(instruction_hdr_t * instructions, meta_data_t meta_data, int client);

bool session_menu_active(int client, struct pollfd * client_poll)
{
    bool session_active = true;

    meta_data_t meta_data = {
        .bytes_received = 0, .bytes_sent = 0, .msg_len = 0, .msg = { 0 }
    };

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
        case SIGNUP:
            printf("\n\nSIGNUP\n\n");
            create_profile(instruction_set, meta_data, client);

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
        // printf("\n\nClient #%d left\n\n", client_poll->fd);
        close(client_poll->fd);
        client_poll->fd *= -1;
    }

    free(instruction_set);
    instruction_set = NULL;

    return session_active;
}

static instruction_hdr_t * receive_instructions(int         client,
                                                meta_data_t meta_data)
{
    instruction_hdr_t * new_instructions =
        (instruction_hdr_t *)calloc(1, sizeof(instruction_hdr_t));

    if (NULL == new_instructions)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    meta_data.bytes_received =
        receive_bytes(client, new_instructions, sizeof(instruction_hdr_t));

    if (ERROR == meta_data.bytes_received)
    {
        free(new_instructions);
        new_instructions = NULL;

        goto EXIT;
    }

    (void)convert_endianess16(&new_instructions->op_code);

    (void)convert_endianess64(&new_instructions->byte_size);

    printf("Instructions: %x, %lu",
           new_instructions->op_code,
           new_instructions->byte_size);

EXIT:

    return new_instructions;
}

void create_profile(instruction_hdr_t * instructions, meta_data_t meta_data, int client)
{
    uint16_t err_code = OP_UNKNOWN;

    if (NULL == instructions)
    {
        goto EXIT;
    }
    
    char sign_up_data[MAX_MSG_LEN];
    
    meta_data.bytes_received = receive_bytes(client, sign_up_data, instructions->byte_size);

    printf("Data Received: %s", sign_up_data);

    err_code = OP_SUCCESS;

EXIT:

    (void)convert_endianess16(&err_code);

    meta_data.bytes_sent = send_bytes(client, &err_code, sizeof(uint16_t));

    if (ERROR == meta_data.bytes_sent)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in send_bytes(): %s\n\n", __func__);
    }

    return;
}

/*** end of file ***/
