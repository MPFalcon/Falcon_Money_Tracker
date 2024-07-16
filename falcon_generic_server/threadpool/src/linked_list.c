#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "linked_list.h"

#define SAME 0
#define ERR  1

// Conceptualized by GeeksForGeeks

// https://www.geeksforgeeks.org/merge-sort-for-linked-list/

/**
 * @brief                   Split main list evenly into sub-lists
 *
 * @param p_head            Reference to the head node
 * @param pp_front_ref      Reference to first sub-list
 * @param pp_back_ref       Reference to second sub-list
 */
static void merge_split(list_node_t *  p_head,
                        list_node_t ** pp_front_ref,
                        list_node_t ** pp_back_ref);

/**
 * @brief               Contains logic on comparing two nodes
 *
 * @param p_first       First node to compare with
 * @param p_second      Second node to compare with
 * @return              Reference to sorted sub-list
 */
static list_node_t * merge(list_node_t * p_first, list_node_t * p_second);


/**
 * @brief               Pieces together sorting logic (driver code)
 *
 * @param pp_head_ref   Reference to head node
 * @return              Completely sorted list
 */
static list_node_t * merge_sort(list_node_t ** pp_head_ref);

void custom_free(void * p_mem_addr)
{
    free(p_mem_addr);

    return;
}

void * default_compare(void * p_value_to_find, void * p_node)
{
    void * p_node_t = NULL;

    if ((NULL == p_value_to_find) || (NULL == p_node))
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    p_node_t = (list_node_t *)p_node;

    int res =
        memcmp(p_value_to_find, ((list_node_t *)p_node_t)->data, sizeof(int));

    if (SAME != res)
    {
        p_node_t = NULL;
    }

EXIT:

    return p_node_t;
}

list_t * list_new(FREE_F p_customfree, CMP_F p_customcmp)
{
    list_t * p_new_list = (list_t *)calloc(1, sizeof(list_t));

    if (NULL == p_new_list)
    {
        (void)print_error("Failed to create new list", __func__);

        goto EXIT;
    }

    p_new_list->head = NULL;
    p_new_list->tail = NULL;
    p_new_list->size = 0;

    p_new_list->customfree = (NULL == p_customfree) ? (FREE_F)custom_free : p_customfree;

    p_new_list->compare_function = (NULL == p_customcmp) ? (CMP_F)default_compare : p_customcmp;

EXIT:

    return p_new_list;
}

int list_push_head(list_t * p_list, void * p_data)
{
    int err = E_FAILURE;

    if ((NULL == p_list) || (NULL == p_data))
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    list_node_t * p_new_node = (list_node_t *)calloc(1, sizeof(list_node_t));

    if (NULL == p_new_node)
    {
        (void)print_error("Failed to create new node", __func__);

        goto EXIT;
    }

    p_new_node->data     = p_data;
    p_new_node->position = 0;

    if (NULL == p_list->head)
    {
        p_list->head       = p_new_node;
        p_list->tail       = p_new_node;
        p_list->tail->next = p_list->head;
        p_list->size++;

        err = E_SUCCESS;

        goto EXIT;
    }

    list_node_t * p_curr_node = p_list->head;

    int check = (1 != p_list->size);

    while ((ERR == check) || (p_curr_node->next != p_list->head))
    {
        p_curr_node->position++;
        p_curr_node = p_curr_node->next;
        check       = E_SUCCESS;
    }

    p_new_node->next = p_list->head;
    p_list->head     = p_new_node;

    p_list->tail->next = p_list->head;
    p_list->size++;

    err = E_SUCCESS;

EXIT:

    return err;
}

int list_push_tail(list_t * p_list, void * p_data)
{
    int err = E_FAILURE;

    if ((NULL == p_list) || (NULL == p_data))
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    list_node_t * p_new_node = (list_node_t *)calloc(1, sizeof(list_node_t));

    if (NULL == p_new_node)
    {
        (void)print_error("Failed to create new node", __func__);

        goto EXIT;
    }

    p_new_node->data     = p_data;
    p_new_node->position = p_list->size;

    if (NULL == p_list->head)
    {
        p_new_node->position = 0;
        p_list->head         = p_new_node;
        p_list->tail         = p_new_node;
        p_list->tail->next   = p_list->head;
        p_list->size++;

        err = E_SUCCESS;

        goto EXIT;
    }

    list_node_t * p_curr_node = p_list->head;

    int check = (1 != p_list->size);

    while ((ERR == check) || (p_curr_node->next != p_list->head))
    {
        p_curr_node = p_curr_node->next;
        check       = E_SUCCESS;
    }

    p_curr_node->next = p_new_node;
    p_new_node->next  = p_list->head;
    p_list->tail      = p_new_node;
    p_list->size++;

    err = E_SUCCESS;

EXIT:

    return err;
}

int list_emptycheck(list_t * p_list)
{
    int empty = E_FAILURE;

    if (NULL == p_list)
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    if (NULL == p_list->head)
    {
        (void)print_note("List is empty", __func__, E_FAILURE);

        goto EXIT;
    }

    empty = E_SUCCESS;

EXIT:

    return empty;
}

list_node_t * list_pop_head(list_t * p_list)
{
    list_node_t * p_curr_node = NULL;

    if (NULL == p_list)
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    if (E_FAILURE == list_emptycheck(p_list))
    {
        goto EXIT;
    }

    p_curr_node = p_list->head;

    if (1 == p_list->size)
    {
        p_list->head = NULL;
        p_list->tail = NULL;
        p_list->size = 0;

        goto EXIT;
    }

    p_list->head       = p_curr_node->next;
    p_list->tail->next = p_list->head;
    p_list->size--;

    p_curr_node->next = NULL;

EXIT:

    return p_curr_node;
}

list_node_t * list_pop_tail(list_t * p_list)
{
    list_node_t * p_curr_node = NULL;

    if (NULL == p_list)
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    int res = list_emptycheck(p_list);

    if (E_FAILURE == res)
    {
        goto EXIT;
    }

    p_curr_node = p_list->head;

    if (1 == p_list->size)
    {
        p_list->head = NULL;
        p_list->tail = NULL;
        p_list->size = 0;

        goto EXIT;
    }

    while (p_curr_node->next != p_list->tail)
    {
        p_curr_node = p_curr_node->next;
    }
    
    p_list->tail = p_curr_node;

    p_curr_node = p_curr_node->next;

    p_list->size--;
    p_list->tail->next = p_list->head;

EXIT:

    return p_curr_node;
}

list_node_t * list_peek_head(list_t * p_list)
{
    list_node_t * p_curr_node = NULL;

    if (NULL == p_list)
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    int res = list_emptycheck(p_list);

    if (E_FAILURE == res)
    {
        goto EXIT;
    }

    p_curr_node = p_list->head;

EXIT:

    return p_curr_node;
}

list_node_t * list_peek_tail(list_t * p_list)
{
    list_node_t * p_curr_node = NULL;

    if (NULL == p_list)
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    int res = list_emptycheck(p_list);

    if (E_FAILURE == res)
    {
        goto EXIT;
    }

    p_curr_node = p_list->tail;

EXIT:

    return p_curr_node;
}

int list_remove(list_t * p_list, void ** pp_item_to_remove)
{
    int      err = E_FAILURE;
    uint32_t idx = 0;

    if ((NULL == p_list) || (NULL == pp_item_to_remove))
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    int res = list_emptycheck(p_list);

    if (E_FAILURE == res)
    {
        goto EXIT;
    }

    list_node_t * p_curr_node     = p_list->head;
    list_node_t * p_deleted_node = NULL;
    list_node_t * p_prev_node     = p_list->head;
    list_node_t * p_next_node     = NULL;

    p_deleted_node = (list_node_t *)p_list->compare_function(
        *pp_item_to_remove, p_list->head);

    if (NULL != p_deleted_node)
    {

        p_list->customfree(list_pop_head(p_list));
        err = E_SUCCESS;

        goto EXIT;
    }

    p_deleted_node = (list_node_t *)p_list->compare_function(
        *pp_item_to_remove, p_list->tail);

    if (NULL != p_deleted_node)
    {
        p_list->customfree(list_pop_tail(p_list));
        err = E_SUCCESS;

        goto EXIT;
    }

    // Accurrate loop to iterate through the list until
    // the node to be deleted has been found

    while ((p_list->size - 1) > idx)
    {
        p_prev_node = p_curr_node;
        p_curr_node = p_curr_node->next;
        p_next_node = p_curr_node->next;

        p_deleted_node = (list_node_t *)p_list->compare_function(
            *pp_item_to_remove, p_curr_node);

        // Following logic insures that the list does not break once the 
        // node to be deleted has been unlinked and freed

        if (NULL != p_deleted_node)
        {
            p_prev_node->next = p_next_node;
            p_list->customfree(p_deleted_node->data);
            p_deleted_node->data = NULL;
            free(p_deleted_node);
            p_deleted_node = NULL;
            p_list->size--;
            p_list->tail->next = p_list->head;

            err = E_SUCCESS;

            break;
        }

        idx++;
    }

    p_curr_node = p_prev_node->next;

    int check = (1 != p_list->size);

    while ((ERR == check) || (p_curr_node->next != p_list->head))
    {
        p_curr_node->position--;
        p_curr_node = p_curr_node->next;
        check       = E_SUCCESS;
    }

    p_list->tail->next = p_list->head;

EXIT:

    return err;
}

int list_foreach_call(list_t * p_list, ACT_F p_action_function)
{
    int      err = E_FAILURE;
    uint32_t idx = 0;

    if ((NULL == p_list) || (NULL == p_action_function))
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    int res = list_emptycheck(p_list);

    if (E_FAILURE == res)
    {
        goto EXIT;
    }

    list_node_t * p_curr_node = p_list->head;

    int check = (1 != p_list->size);

    while ((ERR == check) || (p_curr_node->next != p_list->head))
    {
        p_action_function(p_curr_node);
        p_curr_node = p_curr_node->next;

        idx++;
        check = E_SUCCESS;
    }

    err = E_SUCCESS;

EXIT:

    return err;
}

list_node_t * list_find_first_occurrence(list_t * p_list,
                                         void **  pp_search_data)
{
    list_node_t * p_returned_node = NULL;

    if ((NULL == p_list) || (NULL == pp_search_data))
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    int res = list_emptycheck(p_list);

    if (E_FAILURE == res)
    {
        goto EXIT;
    }

    list_node_t * p_curr_node = p_list->head;
    p_returned_node           = p_list->head;

    int check = (1 != p_list->size);

    while ((ERR == check) || (p_curr_node->next != p_list->head))
    {
        p_returned_node = (list_node_t *)p_list->compare_function(
            *pp_search_data, p_curr_node);

        if (NULL != p_returned_node)
        {
            break;
        }

        p_curr_node = p_curr_node->next;
        check       = E_SUCCESS;
    }

EXIT:

    return p_returned_node;
}

list_t * list_find_all_occurrences(list_t * p_list, void ** pp_search_data)
{
    list_t * p_new_list = NULL;

    if ((NULL == p_list) || (NULL == pp_search_data))
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    int res = list_emptycheck(p_list);

    if (E_FAILURE == res)
    {
        goto EXIT;
    }

    p_new_list = list_new(p_list->customfree, p_list->compare_function);

    if (NULL == p_new_list)
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    list_node_t * p_curr_node     = p_list->head;
    list_node_t * p_passed_node = NULL;
    int           node_check      = E_FAILURE;

    int check = (1 != p_list->size);

    while ((ERR == check) || (p_curr_node->next != p_list->head))
    {
        p_passed_node = (list_node_t *)p_list->compare_function(
            *pp_search_data, p_curr_node);

        if (NULL != p_passed_node)
        {
            node_check = list_push_head(p_new_list, p_passed_node->data);

            if (E_SUCCESS != node_check)
            {
                (void)delete_list(&p_new_list);
                p_new_list = NULL;
                print_error("There was an error adding to nodes new list", __func__);
            }
        }

        p_curr_node = p_curr_node->next;
        check       = E_SUCCESS;
    }

EXIT:

    return p_new_list;
}

int list_sort(list_t * p_list)
{
    int err = E_FAILURE;

    if (NULL == p_list)
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    int res = list_emptycheck(p_list);

    if (E_FAILURE == res)
    {
        err = E_SUCCESS;

        goto EXIT;
    }

    if (1 == p_list->size)
    {
        err = print_note("There is only one element", __func__, E_SUCCESS);

        goto EXIT;
    }

    list_node_t * p_curr_node = p_list->head;

    // Making the list uncircular to simplify merge sort logic

    p_list->tail->next = NULL;
    p_list->head       = merge_sort((list_node_t **)&p_list->head);

    // Reataching the tail to the head properly

    while (NULL != p_curr_node->next)
    {
        p_curr_node = p_curr_node->next;
    }

    p_list->tail       = p_curr_node;
    p_list->tail->next = p_list->head;

    err = E_SUCCESS;

EXIT:

    return err;
}

int list_clear(list_t * p_list)
{
    int err = E_FAILURE;

    if (NULL == p_list)
    {
        goto EXIT;
    }

    int res = list_emptycheck(p_list);

    if (E_FAILURE == res)
    {
        err = E_SUCCESS;

        goto EXIT;
    }

    list_node_t * p_curr_node = p_list->head;
    list_node_t * p_prev_node = p_curr_node;

    while (0 < p_list->size)
    {
        p_curr_node = p_curr_node->next;
        free(p_prev_node);
        p_prev_node = p_curr_node;

        p_list->size--;
    }

    p_list->head = NULL;
    p_list->tail = NULL;

    err = E_SUCCESS;

EXIT:

    return err;
}

int delete_list(list_t ** pp_list_address)
{
    int err = E_FAILURE;

    if (NULL == pp_list_address)
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    if (NULL == *pp_list_address)
    {
        (void)print_note("List has already been deleted", __func__, E_FAILURE);

        goto EXIT;
    }

    err = list_clear(*pp_list_address);

    if (E_SUCCESS != err)
    {
        goto EXIT;
    }

    free(*pp_list_address);
    *pp_list_address = NULL;

    err = E_SUCCESS;

EXIT:

    return err;
}


static void merge_split(list_node_t *  p_head,
                        list_node_t ** pp_front_ref,
                        list_node_t ** pp_back_ref)
{
    if ((NULL == p_head) || (NULL == pp_front_ref) || (NULL == pp_back_ref))
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    list_node_t * p_slow_node = p_head;
    list_node_t * p_fast_node = p_head->next;

    while (p_fast_node != NULL)
    {
        p_fast_node = p_fast_node->next;

        if (p_fast_node != NULL)
        {
            p_slow_node = p_slow_node->next;
            p_fast_node = p_fast_node->next;
        }
    }

    *pp_front_ref     = p_head;
    *pp_back_ref      = p_slow_node->next;
    p_slow_node->next = NULL;

EXIT:

    return;
}

static list_node_t * merge(list_node_t * p_first, list_node_t * p_second)
{
    list_node_t * p_selected_node = NULL;

    // Base case

    if (NULL == p_first)
    {
        p_selected_node = p_second;

        goto EXIT;
    }
    else if (NULL == p_second)
    {
        p_selected_node = p_first;

        goto EXIT;
    }

    // Pick either p_first or p_second and perform recursion

    if (*(int *)p_first->data <= *(int *)p_second->data)
    {
        p_selected_node       = p_first;
        p_selected_node->next = merge(p_first->next, p_second);
    }
    else
    {
        p_selected_node       = p_second;
        p_selected_node->next = merge(p_first, p_second->next);
    }

EXIT:

    return p_selected_node;
}

static list_node_t * merge_sort(list_node_t ** pp_head_ref)
{
    list_node_t * p_head      = *pp_head_ref;
    list_node_t * p_front_ref = NULL;
    list_node_t * p_back_ref  = NULL;

    if (NULL == pp_head_ref)
    {
        (void)detected_null_pointer(__func__);

        goto EXIT;
    }

    // Base case (length of sub-list is 0 or 1)

    if ((NULL == p_head) || (NULL == p_head->next))
    {
        goto EXIT;
    }

    // Split sub-lists

    merge_split(p_head, &p_front_ref, &p_back_ref);

    // Recursivly sort sublists

    merge_sort(&p_front_ref);
    merge_sort(&p_back_ref);

    *pp_head_ref = merge(p_front_ref, p_back_ref);

EXIT:

    return *pp_head_ref;
}

/*** end of file ***/
