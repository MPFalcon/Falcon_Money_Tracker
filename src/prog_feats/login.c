#include "prog_feats/login.h"

int auth_client(int profile_id, char * password, char * username_email)
{
    int err_code = E_FAILURE;

    if ((NULL == password) || (NULL == password))
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}