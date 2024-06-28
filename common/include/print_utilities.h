/** @file     print_utilities.h
 *
 * @brief     Contains helper functions for printing errors
 */

#ifndef PRINT_UTILITIES_H
#    define PRINT_UTILITIES_H

#    include "common_macros.h"

#    ifdef DEBUG
#        define DEBUG_PRINT(...) (void)fprintf(stderr, __VA_ARGS__);
#    else
#        define DEBUG_PRINT(...) (void)0
#    endif

/**
 * @brief                   Print information to the user
 *
 * @param p_note_message    Note message
 * @param p_note_location   Function location
 * @param status            Defines event as an error or not
 *
 * @return                  The status code passed in the status
 *                          parameter
 */
int print_note(const char * p_note_message,
               const char * p_note_location,
               int          status);

/**
 * @brief                   Print error to the user
 *
 * @param p_err_message     Note message
 * @param p_err_location    Function location
 *
 * @return                  FAILURE: 1
 */
int print_error(const char * p_err_message, const char * p_err_location);

/**
 * @brief   Detected parameter as null pointer; Error will be
 *          printed and the program will exit
 *
 * @return  FAILURE: 1
 */
int detected_null_pointer(const char * p_func_name);

#endif /* PRINT_UTILITIES_H */

/*** EOF ***/
