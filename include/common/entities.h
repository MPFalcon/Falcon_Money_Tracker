/** @file entities.h
 *
 * @brief Holds stuctures of common entities within the program
 *
 */

#ifndef ENTITIES_H
#    define ENTITIES_H

#    include <mysql/mysql.h>

#    include "net_io_stream.h"

#    define MAX_BANK_LEN 200
#    define MAX_NAME_LEN 50
#    define MAX_PASS_LEN 100

typedef struct __attribute__((packed)) INSTRUCTION_HEADER
{
    uint16_t op_code;
    uint64_t byte_size;
} instruction_hdr_t;

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
    char     email[MAX_NAME_LEN];
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
    OP_EXIST    = 0xbeeb,
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
instruction_hdr_t * receive_instructions(int client, meta_data_t meta_data);

#endif /* ENTITIES_H */

/*** end of file ***/