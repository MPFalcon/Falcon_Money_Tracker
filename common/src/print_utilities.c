#include "print_utilities.h"

int print_note(const char * p_note_message,
               const char * p_note_location,
               int          status)
{
    if ((NULL == p_note_message) || (NULL == p_note_location))
    {
        status = detected_null_pointer(__func__);

        goto EXIT;
    }

    if (E_SUCCESS == status)
    {
        (void)fprintf(
            stderr, "\n\nNOTE[+]  %s: %s\n\n", p_note_message, p_note_location);
    }
    else
    {
        (void)fprintf(
            stderr, "\n\nNOTE[x]  %s: %s\n\n", p_note_message, p_note_location);
    }

EXIT:

    return status;
}

int print_error(const char * p_err_message, const char * p_err_location)
{
    if ((NULL == p_err_message) || (NULL == p_err_location))
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    (void)fprintf(
        stderr, "\n\nERROR[x]  %s: %s\n\n", p_err_message, p_err_location);

EXIT:

    return E_FAILURE;
}

int detected_null_pointer(const char * p_func_name)
{
    if (NULL == p_func_name)
    {
        goto EXIT;
    }

#ifdef DEBUG
    (void)fprintf(
        stderr, "\n\nERROR [x]  Null Pointer Detected: %s\n\n", p_func_name);
#else
    (void)p_func_name;
#endif

EXIT:

    return E_FAILURE;
}

/*** EOF ***/
