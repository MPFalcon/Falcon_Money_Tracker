#include "net_io_stream.h"

#define MAX_MTU 1500

#define BITS32        32
#define BITS16        16
#define BITS8         8
#define BITS32        32
#define E32_LMASK     0xFF00FF00
#define E32_RMASK     0xFF00FF
#define E64_LMASK     0xFF00FF00FF00FF00ULL
#define E64_RMASK     0x00FF00FF00FF00FFULL
#define E64_LMASK2B   0xFFFF0000FFFF0000ULL
#define E64_RMASK2B   0x0000FFFF0000FFFFULL
#define SIGNED_16MASK 0xFF
#define SIGNED_32MASK 0xFFFF
#define SIGNED_64MASK 0xFFFFFFFFULL

ssize_t receive_bytes(int read_fd, void * buffer, ssize_t num_of_bytes)
{
    ssize_t total_bytes = ERROR;

    errno = 0;

    if (NULL == buffer)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (0 == num_of_bytes)
    {
        DEBUG_PRINT("\n\nERROR [x]  Nothing to receive: %s\n\n", __func__);

        goto EXIT;
    }

    total_bytes = 0;

    int       err     = 0;
    socklen_t err_len = sizeof(err);
    int ret_val = getsockopt(read_fd, SOL_SOCKET, SO_ERROR, &err, &err_len);

    while ((num_of_bytes > total_bytes) && (ERROR != err) && (ERROR != ret_val))
    {
        for (ssize_t byte_idx = 0; MAX_MTU > byte_idx; byte_idx++)
        // JQR Item - 6.12.1 / For loop
        {
            total_bytes +=
                recv(read_fd, ((uint8_t *)buffer + total_bytes), 1, O_NONBLOCK);
            // JQR Item - 6.8.2 and 6.11.6 / Pointer arithmetic is used to read
            // each byte

            if (E_SUCCESS != errno)
            {
                DEBUG_PRINT("\n\nERROR [x]  Error occurred in recv(): %s\n\n",
                            __func__);

                goto EXIT;
            }

            if (num_of_bytes == total_bytes)
            {
                goto EXIT;
            }
        }

        ret_val = getsockopt(read_fd, SOL_SOCKET, SO_ERROR, &err, &err_len);
    }

    if (total_bytes != num_of_bytes)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Receive process has been interrupted: %s\n\n",
            __func__);

        goto EXIT;
    }

EXIT:

    return total_bytes;
}

ssize_t send_bytes(int write_fd, void * buffer, ssize_t num_of_bytes)
{
    ssize_t total_bytes = ERROR;

    errno = 0;

    if (NULL == buffer)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (0 == num_of_bytes)
    {
        DEBUG_PRINT("\n\nERROR [x]  Nothing to send: %s\n\n", __func__);

        goto EXIT;
    }

    total_bytes = 0;

    int       err     = 0;
    socklen_t err_len = sizeof(err);
    int ret_val = getsockopt(write_fd, SOL_SOCKET, SO_ERROR, &err, &err_len);

    while ((num_of_bytes > total_bytes) && (ERROR != err) && (ERROR != ret_val))
    {
        for (ssize_t byte_idx = 0; MAX_MTU > byte_idx; byte_idx++)
        // JQR Item - 6.12.1 / For loop
        {
            total_bytes +=
                send(write_fd, ((uint8_t *)buffer + total_bytes), 1, O_NONBLOCK);
            // JQR Item - 6.8.3 /

            if (E_SUCCESS != errno)
            {
                DEBUG_PRINT("\n\nERROR [x]  Error occurred in send(): %s\n\n",
                            __func__);

                goto EXIT;
            }

            if (num_of_bytes == total_bytes)
            {
                break;
            }
        }

        ret_val = getsockopt(write_fd, SOL_SOCKET, SO_ERROR, &err, &err_len);
    }

    if (total_bytes != num_of_bytes)
    {
        DEBUG_PRINT("\n\nERROR [x]  Send process has been interrupted: %s\n\n",
                    __func__);

        goto EXIT;
    }

EXIT:

    return total_bytes;
}

int convert_endianess16(void * bytes)
{
    int err_code = E_FAILURE;

    if (NULL == bytes)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    uint16_t num = *(uint16_t *)bytes;

    *(uint16_t *)bytes = (num >> BITS8) | (num << BITS8);

    // For signed: *(int16_t *)bytes (num >> BITS8) | ((num << BITS8) &
    // SIGNED_16MASK);

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

int convert_endianess32(void * bytes)
{
    int err_code = E_FAILURE;

    if (NULL == bytes)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    uint32_t num = *(uint32_t *)bytes;

    num = ((num << BITS8) & E32_LMASK) | ((num >> BITS8) & E32_RMASK);

    *(uint32_t *)bytes = (num << BITS16) | (num >> BITS16);

    // For signed: *(int64_t *)bytes = (num << BITS16) | ((num >> BITS16) &
    // SIGNED_32MASK);

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

int convert_endianess64(void * bytes)
{
    int err_code = E_FAILURE;

    if (NULL == bytes)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    uint64_t num = *(uint64_t *)bytes;

    num = ((num << BITS8) & E64_LMASK) | ((num >> BITS8) & E64_RMASK);
    num = ((num << BITS16) & E64_LMASK2B) | ((num >> BITS16) & E64_RMASK2B);

    *(uint64_t *)bytes = (num << BITS32) | (num >> BITS32);

    // For signed: *(int64_t *)bytes = (num << BITS32) | ((num >> BITS32) &
    // SIGNED_64MASK);

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

/** end of file **/
