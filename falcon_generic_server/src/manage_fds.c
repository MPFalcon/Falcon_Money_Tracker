#include "manage_fds.h"

/**
 * @brief           Wrapper function around job that is going to be ran
 * 
 * @param args      Valid session
 * 
 * @return          NULL 
 */
static job_f client_driver(void * args);

int setup_poll(threadpool_t * threadpool, job_f const_func, free_f free_func, void * args, int svr_sock)
{
    int err_code = E_FAILURE;

    if (NULL == threadpool)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    

EXIT:

    return err_code;
}

static job_f client_driver(void * args)
{
    session_t * session = NULL;
    args_t * custom_job = NULL;

    if (NULL == args)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    custom_job = (args_t *)calloc(1, sizeof(args_t));

    if (NULL == custom_job)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    session = (session_t *)args;

    custom_job->client_fd = session->client_poll_fd->fd;
    session->client_poll_fd->fd *= -1;
    custom_job->args = session->args;
    session->associated_job(custom_job);
    session->custom_free(custom_job->args);
    session->client_poll_fd->fd *= -1;

EXIT:

    return NULL;
}

/*** end of file ***/
