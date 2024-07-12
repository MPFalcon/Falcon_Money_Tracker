#ifndef UTILS_H
#define UTILS_H

#define ERROR   (-1)
#define SUCCESS 0

/**
 * @brief A thread job function. The threadpool should operate on a job of this
 * type.
 * @note job_f must know and validate the internals of its singular argument,
 * void * arg. arg may be NULL.
 */
typedef void * (*job_f)(void * arg);

/**
 * @brief a custom free function for an arg
 * @note free_f must be safe if a NULL pointer is provided to it.
 */
typedef void (*free_f)(void * data);

#endif