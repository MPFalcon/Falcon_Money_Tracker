/** @file cli.h
 *
 * @brief Handles command line arguments
 *
 */

#ifndef CLI_H
#    define CLI_H

#    include <getopt.h>
#    include <stdbool.h>

#    include "server_interface.h"

#    define MAX_PORT_NUM         65535
#    define MIN_PORT_NUM         1024
#    define DEFAULT_PORT         31337U

typedef struct options
{
    uint16_t port;
    bool     port_flag;
} options_t;

/**
 * @brief               Handles CLI arguments
 *
 * @param argc          Number of arguments present
 * @param pp_argv       Command line arguments
 * @param p_options     Valid options instance
 *
 * @return              SUCCESS: 0
 *                      FAILURE: 1
 */
int handle_cli(int argc, char ** pp_argv, options_t * p_options);

#endif /* CLI_H */

/*** end of file ***/
