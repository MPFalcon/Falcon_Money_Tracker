#include "prog_feats/signup.h"

typedef union bigRand
{
    uint64_t ll;
    uint32_t ii[2];
} big_ran_t;

/**
 * @brief  Random Number Generator for uint64_t
 *
 * @return Random generated uint64_t
 */
static uint64_t rand64();

profile_t * create_profile(instruction_hdr_t * instructions,
                                  meta_data_t         meta_data,
                                  int                 client)
{
    const int   required_len_count = 3;
    uint16_t    err_code           = OP_UNKNOWN;
    profile_t * new_profile        = NULL;

    uint64_t required_lens[required_len_count];

    if (NULL == instructions)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    new_profile = (profile_t *)calloc(1, sizeof(profile_t));

    if (NULL == new_profile)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    meta_data.bytes_received =
        receive_bytes(client, required_lens, instructions->byte_size);

    for (int idx = 0; required_len_count > idx; idx++)
    {
        (void)convert_endianess64(&required_lens[idx]);
        printf("\n\n%lu\n\n", required_lens[idx]);
    }

    if ((MAX_NAME_LEN < required_lens[0]) ||
        (MAX_PASS_LEN < required_lens[1]) || (MAX_NAME_LEN < required_lens[2]))
    {
        err_code = OP_MSGINVAL;

        goto EXIT;
    }

    meta_data.bytes_received =
        receive_bytes(client, new_profile->username, required_lens[0]);

    meta_data.bytes_received =
        receive_bytes(client, new_profile->password, required_lens[1]);

    meta_data.bytes_received =
        receive_bytes(client, new_profile->email, required_lens[2]);

    printf("\n\nData Received -\n\nUsername: %s\nPassword: %s\nEmail: %s\n\n",
           new_profile->username,
           new_profile->password,
           new_profile->email);

    new_profile->profile_id = rand64();

    (void)convert_endianess64(&new_profile->profile_id);

    meta_data.bytes_sent =
        send_bytes(client, &new_profile->profile_id, sizeof(uint64_t));

    // Add to database

    

    err_code = OP_SUCCESS;

EXIT:

    (void)convert_endianess16(&err_code);

    meta_data.bytes_sent = send_bytes(client, &err_code, sizeof(uint16_t));

    if (ERROR == meta_data.bytes_sent)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in send_bytes(): %s\n\n",
                    __func__);
    }

    return new_profile;
}

static uint64_t rand64()
{
    big_ran_t bytes;
    bytes.ii[0] = rand();
    bytes.ii[1] = rand();

    return bytes.ll;
}