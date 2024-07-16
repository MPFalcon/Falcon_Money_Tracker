#include "cli.h"

#define MIN_THREADS      1
#define MAX_THREADS      100
#define MIN_ARGS         1
#define MAX_ARGS         5
#define DEFAULT_NUM_BASE 10U

/**
 * @brief                           Specificaly handle the argument that
 *                                  is associated with -n option
 *
 * @param p_requested_thread_count  Requested thread count passed by
 *                                  reference that is due to change
 *
 * @return                          SUCCESS: 0
 *                                  FAILURE: 1
 */
static int handle_n_argument(uint32_t * p_requested_thread_count,
                             bool *     p_flag);

/**
 * @brief                           Specificaly handle the argument that
 *                                  is associated with -p option
 *
 * @param p_requested_port          Requested port number passed by
 *                                  reference that is due to change
 *
 * @return                          SUCCESS: 0
 *                                  FAILURE: 1
 */
static int handle_p_argument(uint16_t * p_requested_port, bool * p_flag);

/**
 * @brief Print help
 *
 */
static void print_help();

int handle_cli(int argc, char ** pp_argv, options_t * p_options)
{
    int err_code = E_FAILURE;
    int opt      = 0;

    if ((NULL == p_options) || (NULL == pp_argv))
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if ((MAX_ARGS < argc) || (MIN_ARGS > argc))
    {
        print_help();

        goto EXIT;
    }

    while ((opt = getopt(argc, pp_argv, "-n:p:h")) != -1)
    {
        switch (opt)
        {
            case 'n':
                err_code = handle_n_argument(&p_options->threads,
                                             &p_options->threads_flag);
                break;
            case 'p':
                err_code =
                    handle_p_argument(&p_options->port, &p_options->port_flag);
                break;
            default:
                print_help();

                goto EXIT;

                break;
        }

        if (SUCCESS != err_code)
        {
            DEBUG_PRINT(
                "\n\nERROR [x]  Error occurred in handle_%c_argument(): %s\n\n",
                (char)opt,
                __func__);

            goto EXIT;
        }
    }

    printf(
        "\n\nThere will be %u threads running in the background and server "
        "will be running on port %hu\n\n",
        p_options->threads,
        p_options->port);

    err_code = SUCCESS;

EXIT:

    return err_code;
}

static int handle_n_argument(uint32_t * p_requested_thread_count, bool * p_flag)
{
    int err_code = E_FAILURE;

    char * p_restricted_char = NULL;

    if ((NULL == p_requested_thread_count) || (NULL == p_flag))
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (true == *p_flag)
    {
        (void)print_note("Duplicate option detected", __func__, E_FAILURE);

        goto EXIT;
    }

    if (NULL == optarg)
    {
        print_help();

        goto EXIT;
    }

    *p_requested_thread_count =
        strtoul(optarg, &p_restricted_char, DEFAULT_NUM_BASE);

    if (('\0' != *p_restricted_char) || (errno != 0))
    {
        (void)print_note(
            "Thread count must be an integral value", __func__, E_FAILURE);

        goto EXIT;
    }

    if ((MAX_THREADS < *p_requested_thread_count) ||
        (MIN_THREADS >= *p_requested_thread_count))
    {
        (void)fprintf(
            stderr,
            "\n\nNOTE [x]  Thread count must be between %d and %d: %s\n\n",
            MIN_THREADS,
            MAX_THREADS,
            __func__);

        goto EXIT;
    }

    *p_flag = true;

    err_code = SUCCESS;

EXIT:

    return err_code;
}

static int handle_p_argument(uint16_t * p_requested_port, bool * p_flag)
{
    int err_code = E_FAILURE;
    unsigned long restricted_int = 0;
    
    char * p_restricted_char = NULL;

    if ((NULL == p_requested_port) || (NULL == p_flag))
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (true == *p_flag)
    {
        (void)print_note("Duplicate option detected", __func__, E_FAILURE);

        goto EXIT;
    }

    if (NULL == optarg)
    {
        print_help();

        goto EXIT;
    }

    restricted_int =
        strtol(optarg, &p_restricted_char, DEFAULT_NUM_BASE);

    if (('\0' != *p_restricted_char) || (errno != 0))
    {
        (void)print_note(
            "Port number must be an integral value", __func__, E_FAILURE);

        goto EXIT;
    }

    if ((MAX_PORT_NUM < restricted_int) || (MIN_PORT_NUM > restricted_int))
    {
        (void)fprintf(
            stderr,
            "\n\nNOTE [x]  Port number must be between %d and %d: %s\n\n",
            MIN_PORT_NUM,
            MAX_PORT_NUM,
            __func__);

        goto EXIT;
    }

    *p_requested_port = (uint16_t)restricted_int;
    *p_flag           = true;

    err_code = SUCCESS;

EXIT:

    return err_code;
}

static void print_help()
{
    printf(
        "\n\n   Falcon Money Tracker Server\n"
        "---------------------------------\n\n"
        "money_track_server [-p PORT] [-h]\n\n"
        "-h  Show this message\n"
        "-p  PORT\n"
        "-n  NUM\n\n");
}

/*** end of file ***/
