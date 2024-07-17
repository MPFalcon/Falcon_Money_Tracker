#include "cli.h"

int main(int argc, char ** argv)
{
    int err_code = EXIT_FAILURE;

    options_t options = { .threads      = DEFAULT_THREADS,
                          .port         = DEFAULT_PORT,
                          .port_flag    = false,
                          .threads_flag = false };

    config_t server_config = { 0 };

    err_code = signal_action_setup();

    if (ERROR == err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Error occurred in signal_action_setup() : %s",
            __func__);

        goto EXIT;
    }

    err_code = handle_cli(argc, argv, &options);

    if (E_FAILURE == err_code)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Error occurred in signal_action_setup() : %s",
            __func__);

        goto EXIT;
    }

    server_config.port                = options.port;
    server_config.thread_count        = options.threads;
    server_config.requested_free_func = (free_f)free_session;
    server_config.timeout             = 50;

    err_code = setup_session(&server_config);

    if (E_FAILURE == err_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in setup_session() : %s",
                    __func__);

        goto EXIT;
    }

    err_code = EXIT_SUCCESS;

EXIT:

    return err_code;
}