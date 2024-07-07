/** @file login.h
 *
 * @brief Handles aspects of the login system
 *
 */

#ifndef LOGIN_H
#    define LOGIN_H

#    include "common/entities.h"

int auth_client(int profile_id, char * password, char * username_email);

#endif /* LOGIN_H */

/*** end of file ***/
