/** @file login.h
 *
 * @brief Handles aspects of the sign up system
 *
 */

#ifndef SIGNUP_H
#    define SIGNUP_H

#    include "common/entities.h"

/**
 * @brief               Create a profile and add to database
 *
 * @param instructions  Valid instructions instant
 * @param meta_data     Metadata structure
 * @param client        Client FD
 *
 * @return              Valid profile instance
 */
profile_t * create_profile(instruction_hdr_t * instructions,
                           meta_data_t         meta_data,
                           int                 client);

#endif /* SIGNUP_H */

/*** end of file ***/
