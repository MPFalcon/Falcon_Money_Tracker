#include "../include/threadpool.h"
#include "print_utilities.h"

#define STOP             0x1
#define SHUTDOWN         0x2
#define START            0x4
#define BACKLOG_CAPACITY 20

/**
 * @brief   Data structure for queue node
 *
 */
typedef struct job_data
{
    job_f  arg_job;
    free_f arg_customfree;
    void * params;
} job_data_t;

/**
 * @brief               Custom free for queue to use when clearing/
 *                      destroying
 *
 * @param p_mem_addr    Provided struct to free
 */
static void threadpool_custom_free(void * p_mem_addr);

/**
 * @brief               Containing logic that properly spins up threads and
 *                      conducts graceful shutdown if threads fail
 *
 * @param thread_count  How many threads to spin up
 * @param p_pool        Valid thread pool instance
 *
 * @return              SUCCESS: 0
 *                      ERROR:   1
 */
static int initialize_threads(uint64_t thread_count, threadpool_t * p_pool);

/**
 * @brief           Function called in a thread has been initialized
 *
 * @param p_args    Provided arguments
 *
 * @return          Pointer to the function for a single thread
 */
static void * start_task(void * p_args);

/**
 * @brief           Function for holding thread until queue is not empty
 *                  or if shutdown flag is triggered
 *
 * @param p_pool    Valid threadpool instance
 *
 * @return          SUCCESS: 0
 *                  ERROR:   1
 */
static int thread_wait(threadpool_t * p_pool);

/**
 * @brief                   Relieves overhead for setting up
 *                          thread pool to aid readability
 *
 * @param p_new_pool        Valid thread pool instance
 * @param thread_count      Requested number of threads to spin up
 *
 * @return                  SUCCESS: 0
 *                          ERROR:   1
 */
static int setup_thread_pool(threadpool_t * p_new_pool, size_t thread_count);

threadpool_t * threadpool_create(size_t thread_count)
{
    int err_code = ERROR;

    threadpool_t * p_new_pool = (threadpool_t *)calloc(1, sizeof(threadpool_t));

    if (NULL == p_new_pool)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Something went wrong creating a new thread pool: "
            "%s\n\n",
            __func__);

        goto EXIT;
    }

    p_new_pool->pool_list =
        list_new((FREE_F)threadpool_custom_free, NULL);

    if (NULL == p_new_pool->pool_list)
    {
        DEBUG_PRINT(
            "\n\nERROR [x]  Something wrong creating a new thread pool: %s\n\n",
            __func__);

        free(p_new_pool);
        p_new_pool = NULL;

        goto EXIT;
    }

    err_code = setup_thread_pool(p_new_pool, thread_count);

    if (SUCCESS != err_code)
    {
        p_new_pool = NULL;
    }

EXIT:

    return p_new_pool;
}

int threadpool_shutdown(threadpool_t * pool_p)
{
    int err_code = ERROR;

    if (NULL == pool_p)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    pthread_mutex_lock(&pool_p->p_mutex_id);
    pool_p->exit_flag = STOP;

    pthread_cond_broadcast(&pool_p->pool_processing_cond);
    pthread_mutex_unlock(&pool_p->p_mutex_id);

    for (size_t idx = 0; pool_p->capacity > idx; idx++)
    {
        err_code = pthread_join(pool_p->threads[idx], NULL);

        if (E_SUCCESS != err_code)
        {
            DEBUG_PRINT("\n\nERROR [x]  Failed to join thread: %s\n\n",
                        __func__);

            goto EXIT;
        }
    }

    pthread_mutex_destroy(&pool_p->p_mutex_id);
    pthread_cond_destroy(&pool_p->pool_processing_cond);

    pool_p->exit_flag = (STOP | SHUTDOWN);

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

int threadpool_destroy(threadpool_t ** pool_pp)
{
    int err_code = ERROR;

    if (NULL == pool_pp)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (NULL == *pool_pp)
    {
        DEBUG_PRINT(
            "\n\nERROR[x] Thread Pool has already been destroyed: %s\n\n",
            __func__);

        goto EXIT;
    }

    if (0 == (SHUTDOWN & (*pool_pp)->exit_flag))
    {
        threadpool_shutdown((*pool_pp));
    }

    list_delete(&((*pool_pp)->pool_list));

    free((*pool_pp)->threads);
    (*pool_pp)->threads = NULL;

    free(*pool_pp);
    *pool_pp = NULL;

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

int threadpool_add_job(threadpool_t * pool,
                       job_f          job,
                       free_f         del_f,
                       void *         arg_p)
{
    int err_code = ERROR;

    if ((NULL == pool) || (NULL == job))
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    if (0 != (SHUTDOWN & pool->exit_flag))
    {
        DEBUG_PRINT("\n\nERROR [x]  Thread pool has shut down: %s\n\n",
                    __func__);

        goto EXIT;
    }

    job_data_t * data_set = (job_data_t *)calloc(1, sizeof(job_data_t));

    if (NULL == data_set)
    {
        DEBUG_PRINT("\n\nERROR [x]  Failed to allocate memory: %s\n\n",
                    __func__);

        goto EXIT;
    }

    data_set->arg_job        = job;
    data_set->arg_customfree = del_f;
    data_set->params         = arg_p;

    pthread_mutex_lock(&pool->p_mutex_id);
    err_code = list_push_tail(pool->pool_list, data_set);

    if (E_SUCCESS != err_code)
    {
        DEBUG_PRINT("\n\nERROR [x]  Failed to enqueue new job: %s\n\n",
                    __func__);
        pool->pool_list->customfree(data_set);
        pthread_mutex_unlock(&pool->p_mutex_id);

        goto EXIT;
    }

    pthread_cond_signal(&pool->pool_processing_cond);
    pthread_mutex_unlock(&pool->p_mutex_id);

EXIT:

    return err_code;
}

static void threadpool_custom_free(void * p_mem_addr)
{
    if (NULL == p_mem_addr)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    job_data_t * p_job_addr = (job_data_t *)p_mem_addr;

    if (NULL != p_job_addr->arg_customfree)
    {
        p_job_addr->arg_customfree(p_job_addr->params);
    }

    free(p_job_addr);

EXIT:

    return;
}

static int initialize_threads(uint64_t thread_count, threadpool_t * p_pool)
{
    int    err_code = ERROR;
    size_t stop_idx = 0;

    for (size_t idx = 0; thread_count > idx; idx++)
    {
        err_code =
            pthread_create(&p_pool->threads[idx], NULL, start_task, p_pool);

        if (E_SUCCESS != err_code)
        {
            DEBUG_PRINT("\n\nERROR [x]  Failed to create thread: %s\n\n",
                        __func__);
            stop_idx = idx;

            break;
        }
    }

    // If one of the threads fail to initiate, shut down the successful threads
    // up to that point

    if (E_SUCCESS != err_code)
    {
        for (size_t idx = 0; stop_idx > idx; idx++)
        {
            err_code = pthread_join(p_pool->threads[idx], NULL);

            if (E_SUCCESS != err_code)
            {
                DEBUG_PRINT("\n\nERROR [x]  Failed to join thread: %s\n\n",
                            __func__);

                goto EXIT;
            }
        }
    }

EXIT:

    return err_code;
}

static void * start_task(void * p_args)
{
    int err_code = ERROR;

    if (NULL == p_args)
    {
        goto EXIT;
    }

    threadpool_t * p_pool     = (threadpool_t *)p_args;
    job_data_t *   p_ret_data = NULL;
    list_node_t * p_ret_node = NULL;

    pthread_mutex_lock(&p_pool->p_mutex_id);

    for (;;)
    {
        // Function is called in the while loop to allow threads to accurately
        // check if queue is empty

        if ((0 != (STOP & p_pool->exit_flag)) &&
            (E_SUCCESS != list_emptycheck(p_pool->pool_list)))
        {
            break;
        }

        err_code = thread_wait(p_pool);

        if (E_SUCCESS != err_code)
        {
            break;
        }

        p_ret_node = list_pop_head(p_pool->pool_list);
        pthread_mutex_unlock(&p_pool->p_mutex_id);

        if (NULL != p_ret_node)
        {
            p_ret_data = (job_data_t *)p_ret_node->data;
            p_ret_data->arg_job(p_ret_data->params);
            p_pool->pool_list->customfree(p_ret_data);
            free(p_ret_node);
        }
        pthread_mutex_lock(&p_pool->p_mutex_id);
    }
    pthread_mutex_unlock(&p_pool->p_mutex_id);

    p_ret_data = NULL;
    p_ret_node = NULL;

EXIT:

    return NULL;
}

static int thread_wait(threadpool_t * p_pool)
{
    int err_code = ERROR;

    if (NULL == p_pool)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    // Function is called in the while loop to allow threads to accurately
    // check if queue is empty

    while ((E_SUCCESS != list_emptycheck(p_pool->pool_list)) &&
           (0 != (START & p_pool->exit_flag)))
    {
        err_code = pthread_cond_wait(&p_pool->pool_processing_cond,
                                     &p_pool->p_mutex_id);

        if (E_SUCCESS != err_code)
        {
            DEBUG_PRINT(
                "\n\nERROR [x]  Something went wrong waiting for task: %s\n\n",
                __func__);
        }
    }

    err_code = E_SUCCESS;

EXIT:

    return err_code;
}

static int setup_thread_pool(threadpool_t * p_new_pool, size_t thread_count)
{
    int err_code = ERROR;

    if (NULL == p_new_pool)
    {
        DEBUG_PRINT("\n\nERROR [x]  Null Pointer Detected: %s\n\n", __func__);

        goto EXIT;
    }

    p_new_pool->capacity   = thread_count;
    p_new_pool->exit_flag  = START;
    p_new_pool->p_mutex_id = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    p_new_pool->pool_processing_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    pthread_mutex_init(&p_new_pool->p_mutex_id, NULL);
    pthread_cond_init(&p_new_pool->pool_processing_cond, NULL);

    p_new_pool->threads = (pthread_t *)calloc(thread_count, sizeof(pthread_t));

    if (NULL == p_new_pool->threads)
    {
        DEBUG_PRINT("\n\nERROR [x]  Failed to create threads: %s\n\n",
                    __func__);

        list_delete(&p_new_pool->pool_list);

        free(p_new_pool);

        goto EXIT;
    }

    err_code = initialize_threads(thread_count, p_new_pool);

    if (E_SUCCESS != err_code)
    {
        list_delete(&p_new_pool->pool_list);

        free(p_new_pool->threads);
        p_new_pool->threads = NULL;

        free(p_new_pool);
    }

EXIT:

    return err_code;
}

/*** end of file ***/
