#include "signals.h"
#include "print_utilities.h"

#define SIGNAL_IGNORED    (sig_atomic_t)0
#define SIGUSR1_TRIGGERED (sig_atomic_t)1
#define SIGINT_TRIGGERED  (sig_atomic_t)2

volatile sig_atomic_t signal_flag_g = 0;

void signal_handler(int signal)
{
    switch (signal)
    {
        case SIGINT:
            DEBUG_PRINT("\n\nNOTE [x]  SIGINT detected - Commencing graceful shutdown... : %s", __func__);
            signal_flag_g = SIGINT_TRIGGERED;

            break;
        case SIGUSR1:
            DEBUG_PRINT("\n\nNOTE [x]  SIGUSR1 detected - Commencing graceful shutdown... : %s", __func__);
            signal_flag_g = SIGINT_TRIGGERED;

            break;
        case SIGPIPE:
            DEBUG_PRINT("\n\nNOTE [x]  SIGPIPE detected - Commencing graceful shutdown... : %s", __func__);
            signal_flag_g = SIGINT_TRIGGERED;

            break;
        default:
            signal_flag_g = SIGNAL_IGNORED;

            break;
    }
}

int signal_action_setup(void)
{
    int err_code = ERROR;

    struct sigaction action = { 0 };

    action.sa_handler = signal_handler;

    err_code = sigaction(SIGINT, &action, NULL);

    if (E_SUCCESS != err_code)
    {
        goto EXIT;
    }

    err_code = sigaction(SIGUSR1, &action, NULL);

    if (E_SUCCESS != err_code)
    {
        goto EXIT;
    }

    err_code = sigaction(SIGPIPE, &action, NULL);

    if (E_SUCCESS != err_code)
    {
        goto EXIT;
    }

EXIT:

    return err_code;
}

/*** end of file ***/
