/** @file net_io_stream.h
 *
 * @brief Module's purpose is to process read and write operations
 *
 */

#ifndef NET_IO_STREAM_H
#    define NET_IO_STREAM_H

#    include <netinet/in.h>
#    include <poll.h>
#    include <netdb.h>
#    include <sys/socket.h>
#    include <sys/types.h>

#    include "print_utilities.h"
#    include "threadpool.h"
#    include "signals.h"

#    define SIGNAL_IGNORED 0
#    define MAX_MSG_LEN    4098
typedef struct net_meta_data
{
    ssize_t bytes_received;
    ssize_t bytes_sent;
    ssize_t msg_len;
    char    msg[MAX_MSG_LEN];
} meta_data_t;

/**
 * @brief               General receive function for safe and modular operations
 *
 * @param read_fd       File descriptor
 * @param buffer        Buffer to read to
 * @param num_of_bytes  Size of the buffer
 *
 * @return              How many bytes recieved (-1 if process failed)
 */
ssize_t receive_bytes(int read_fd, void * buffer, ssize_t num_of_bytes);

/**
 * @brief               General send function for safe and modular operations
 *
 * @param write_fd      File descriptor
 * @param buffer        Buffer to write from
 * @param num_of_bytes  Size of the buffer
 *
 * @return              How many bytes sent (-1 if process failed)
 */
ssize_t send_bytes(int write_fd, void * buffer, ssize_t num_of_bytes);

/**
 * @brief       Convert byte order in 16 bits
 *
 *              Conceptualized by chmike from StackOverflow
 *
 *              https://stackoverflow.com/questions/2182002/how-to-convert-big-endian-to-little-endian-in-c-without-using-library-functions
 *
 * @param bytes Object in memory
 *
 * @return      SUCCESS: 0
 *              FAILURE: 1
 */
int convert_endianess16(void * bytes);

/**
 * @brief       Convert byte order in 32 bits
 *
 *              Conceptualized by chmike from StackOverflow
 *
 *              https://stackoverflow.com/questions/2182002/how-to-convert-big-endian-to-little-endian-in-c-without-using-library-functions
 *
 * @param bytes Object in memory
 *
 * @return      SUCCESS: 0
 *              FAILURE: 1
 */
int convert_endianess32(void * bytes);

/**
 * @brief       Convert byte order in 64 bits
 *
 *              Conceptualized by chmike from StackOverflow
 *
 *              https://stackoverflow.com/questions/2182002/how-to-convert-big-endian-to-little-endian-in-c-without-using-library-functions
 *
 * @param bytes Object in memory
 *
 * @return      SUCCESS: 0
 *              FAILURE: 1
 */
int convert_endianess64(void * bytes);

/**
 * @brief       Safe wrapper function for close()
 * 
 * @param fd    File descriptor
 */
void safe_close(int file_fd);

#endif /* NET_IO_STREAM_H */

/*** end of file ***/
