#include "common/entities.h"

instruction_hdr_t * receive_instructions(int client, meta_data_t meta_data)
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

/*** end of file ***/
