#include "cli.h"

#define MIN_ARGS         1
#define MAX_ARGS         5
#define DEFAULT_NUM_BASE 10U

/**
 * @brief                           Specifically handle the argument that
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

    while ((opt = getopt(argc, pp_argv, "-p:h")) != -1)
    {
        switch (opt)
        {
            case 'p':
                err_code =
                    handle_p_argument(&p_options->port, &p_options->port_flag);
                break;
            default:
                print_help();
                err_code = E_FAILURE;

                goto EXIT;
                break;
        }

        if (E_SUCCESS != err_code)
        {
            DEBUG_PRINT(
                "\n\nERROR [x]  Error occurred in handle_%c_argument(): %s\n\n",
                (char)opt,
                __func__);

            goto EXIT;
        }
    }

    printf("\n\nServer running on port %hu\n\n", p_options->port);

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static int handle_p_argument(uint16_t * p_requested_port, bool * p_flag)
{
    int err_code = E_FAILURE;

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

    unsigned long restricted_int =
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

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static void print_help()
{
    (void)print_note(
        "Command Line Examples: 'netcalc -p PORT -n NUM' or "
        "'netcalc -h' to print this help message\n"
        "(default thread count is 4 and default port is 31337)",
        __func__,
        E_FAILURE);
}

/*** end of file ***/
