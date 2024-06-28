#include "cli.h"

int main(int argc, char ** argv)
{
    int err_code = EXIT_FAILURE;

    options_t options = {
        .port      = DEFAULT_PORT,
        .port_flag = false
    };

    err_code = signal_action_setup();

    if (ERROR == err_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in signal_action_setup() : %s", __func__);

        goto EXIT;
    }

    err_code = handle_cli(argc, argv, &options);

    if (E_FAILURE == err_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in signal_action_setup() : %s", __func__);

        goto EXIT;
    }

    err_code = setup_driver(options.port);

    if (E_FAILURE == err_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Error occurred in signal_action_setup() : %s", __func__);

        goto EXIT;
    }

    err_code = EXIT_SUCCESS;

EXIT:

    return err_code;
}