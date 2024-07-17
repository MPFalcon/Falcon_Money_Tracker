#include "common/packet_manager.h"

#define HDR_LEN sizeof(header_t)

void * recieve_data(int client, meta_data_t meta_data)
{
    void *     master_buffer = NULL;
    header_t * curr_header   = NULL;
    uint64_t   offset        = 0;

    curr_header = (header_t *)calloc(1, HDR_LEN);

    if (NULL == curr_header)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    meta_data.bytes_received = receive_bytes(client, curr_header, HDR_LEN);

    if (ERROR == meta_data.bytes_received)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in receive_bytes() : %s\n\n",
                    __func__);

        goto EXIT;
    }

    (void)convert_endianess32(&curr_header->seq_num);
    (void)convert_endianess64(&curr_header->total_size);
    (void)convert_endianess64(&curr_header->byte_size);
    (void)convert_endianess64(&curr_header->total_packets);

    // printf("\n\nData Buffer Length: %lu\n\n", sizeof(header_t));

    // printf(
    //     "\n\nHeader Detail - \nSequence Num #%d\nTotal Size: %lu\nBytes Size: "
    //     "%lu\nTotal Packets: %lu\n\n",
    //     curr_header->seq_num,
    //     curr_header->total_size,
    //     curr_header->byte_size,
    //     curr_header->total_packets);

    master_buffer = calloc(1, (curr_header->total_size + 1));

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
            DEBUG_PRINT(
                "\n\nERROR [x]  Error occurred in receive_bytes() : %s\n\n",
                __func__);

            continue;
        }

        (void)convert_endianess32(&curr_header->seq_num);
        (void)convert_endianess64(&curr_header->total_size);
        (void)convert_endianess64(&curr_header->byte_size);
        (void)convert_endianess64(&curr_header->total_packets);

        if (0 < curr_header->byte_size)
        {
            memcpy(((uint8_t *)master_buffer + offset),
                   &curr_header->bytes,
                   curr_header->byte_size);

            offset += curr_header->byte_size;
        }
    }

EXIT:

    return master_buffer;
}

int send_data(int         client,
              meta_data_t meta_data,
              void *      buffer,
              uint64_t    num_of_bytes)
{
    int      err_code = E_FAILURE;
    uint64_t offset   = 0;

    header_t out_going_header = {
        .seq_num       = rand(),
        .total_size    = num_of_bytes,
        .byte_size     = 0,
        .total_packets = (num_of_bytes / DEFAULT_BUFFER_SIZE),
        .bytes         = { 0 },
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
            memset(&out_going_header.bytes, 0x00, DEFAULT_BUFFER_SIZE);
            out_going_header.byte_size = (out_going_header.total_size - offset);
            memcpy(&out_going_header.bytes,
                   ((uint8_t *)buffer + offset),
                   out_going_header.byte_size);
        }
        else
        {
            out_going_header.byte_size = DEFAULT_BUFFER_SIZE;
            memcpy(&out_going_header.bytes,
                   ((uint8_t *)buffer + offset),
                   out_going_header.byte_size);
            offset += out_going_header.byte_size;
        }

        (void)convert_endianess32(&out_going_header.seq_num);
        (void)convert_endianess64(&out_going_header.total_size);
        (void)convert_endianess64(&out_going_header.byte_size);
        (void)convert_endianess64(&out_going_header.total_packets);

        meta_data.bytes_received =
            send_bytes(client, &out_going_header, HDR_LEN);

        if (ERROR == meta_data.bytes_received)
        {
            DEBUG_PRINT(
                "\n\nERROR [x]  Error occurred in send_bytes() : %s\n\n",
                __func__);

            continue;
        }

        (void)convert_endianess32(&out_going_header.seq_num);
        (void)convert_endianess64(&out_going_header.total_size);
        (void)convert_endianess64(&out_going_header.byte_size);
        (void)convert_endianess64(&out_going_header.total_packets);
    }

EXIT:

    return err_code;
}