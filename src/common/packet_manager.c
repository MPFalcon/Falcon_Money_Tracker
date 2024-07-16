#include "packet_manager.h"

#define HDR_LEN sizeof(header_t)

void * recieve_data(int client, meta_data_t meta_data)
{
    void * master_buffer = NULL;
    header_t * curr_header = NULL;
    uint64_t   offset = 0;

    curr_header = (header_t *)calloc(1, HDR_LEN);

    if (NULL == curr_header)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    meta_data.bytes_received = receive_bytes(client, curr_header, HDR_LEN);

    if (ERROR == meta_data.bytes_received)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in receive_bytes() : %s\n\n", __func__);

        goto EXIT;
    }

    master_buffer = calloc(1, curr_header->total_size);

    if (NULL == master_buffer)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    memcpy(master_buffer, &curr_header->bytes, curr_header->byte_size);

    offset += curr_header->byte_size;

    for (uint64_t idx = 1; curr_header->total_packets > idx; idx++)
    {
        meta_data.bytes_received = receive_bytes(client, curr_header, HDR_LEN);

        if (ERROR == meta_data.bytes_received)
        {
            DEBUG_PRINT("\n\nERROR [x]  Error occurred in receive_bytes() : %s\n\n", __func__);

            continue;
        }

        memcpy(((uint8_t *)master_buffer + offset), &curr_header->bytes, curr_header->byte_size);

        offset += curr_header->byte_size;
    }

EXIT:

    return master_buffer;
}

int send_data(int client, meta_data_t meta_data, void * buffer, uint64_t num_of_bytes)
{
    int err_code = E_FAILURE;
    uint64_t offset = 0;

    header_t out_going_header = {
        .seq_num = rand(),
        .total_size = num_of_bytes,
        .byte_size = 0,
        .total_packets = (num_of_bytes / DEFAULT_BUFFER_SIZE),
        .bytes = { 0 },
    };

    if (NULL == buffer)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }



    for (uint64_t idx = 0; out_going_header.total_packets > idx; idx++)
    {
        if (out_going_header.total_packets == (idx + 1))
        {
            out_going_header.byte_size = (out_going_header.total_size - offset);
            memcpy(&out_going_header.bytes, ((uint8_t *)buffer + offset), out_going_header.byte_size);
        }
        else
        {
            out_going_header.byte_size = DEFAULT_BUFFER_SIZE;
            memcpy(&out_going_header.bytes, ((uint8_t *)buffer + offset), out_going_header.byte_size);
            offset += out_going_header.byte_size;
        }

        meta_data.bytes_received = receive_bytes(client, &out_going_header, HDR_LEN);

        if (ERROR == meta_data.bytes_received)
        {
            DEBUG_PRINT("\n\nERROR [x]  Error occurred in receive_bytes() : %s\n\n", __func__);

            continue;
        }
    }

EXIT:

    return err_code;
}