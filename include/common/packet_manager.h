/** @file packet_manager.h
 *
 * @brief Holds stuctures of common entities within the program
 *
 */

#ifndef PACKET_MANAGER_H
#    define PACKET_MANAGER_H

#    include "net_io_stream.h"

#    define DEFAULT_BUFFER_SIZE 1000

typedef union data
{
    __uint128_t unsign128;
    uint64_t unsign64;
    uint32_t unsign32;
    uint16_t unsign16;
    __int128_t sign128;
    int64_t  sign64;
    int32_t  sign32;
    int16_t  sign16;
    uint8_t  data_buffer[DEFAULT_BUFFER_SIZE];
} data_t;


typedef struct falcon_header
{
    int32_t   seq_num;
    uint64_t  total_size;
    uint64_t  byte_size;
    uint64_t  total_packets;
    data_t    bytes;
} header_t;

void * recieve_data(int client, meta_data_t meta_data);

/**
 * @brief               Send packets to client
 * 
 * @param client        Client FD
 * @param meta_data     Metadata structure
 * @param buffer        Data to send
 * @param num_of_bytes  Total size of the data
 * 
 * @return              SUCCESS: 0
 *                      FAILURE: 1 
 */
int send_data(int client, meta_data_t meta_data, void * buffer, uint64_t num_of_bytes);

#endif /* PACKET_MANAGER_H */

/*** end of file ***/