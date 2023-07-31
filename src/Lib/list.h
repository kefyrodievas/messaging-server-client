#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

// Defines linked list
#define define_list(type)\
    typedef struct                                                                                     \
    {                                                                                                  \
        void (*append)(struct element_##type *, type);                                                 \
        void (*prepend)(struct element_##type **, type);                                               \
        int (*remove_last)(struct element_##type *);                                                   \
        int (*remove_first)(struct element_##type **);                                                 \
        int (*remove_by_index)(struct element_##type, int);                                            \
    } _list_functions_##type;  \
                                                                            \
    typedef struct _element_##type                                                                     \
    {                            \
        _list_functions_##type *funcs;                                                                      \
        type val;                                                                                      \
        struct _element_##type *next;                                                                          \
    } element_##type;                                                                                  \
                                                                            \
                                                                                                       \
    element_##type * _create_list_##type()                                                              \
    {                                                                                                  \
        element_##type *head = NULL;                                                                   \
        head = (element_##type *)malloc(sizeof(element_##type));                                       \
        return head;                                                                                   \
    }                                                                                                  \
                                                                                                       \
    void _append_##type(element_##type *head, type value)                                              \
    {                                                                                                  \
        element_##type *current = head;                                                                \
        if (head == NULL)                                                                              \
        {                                                                                              \
            head->val = value;                                                                         \
            head->next = NULL;                                                                         \
            return;                                                                                    \
        }                                                                                              \
        else                                                                                           \
        {                                                                                              \
            while (current->next != NULL)                                                              \
            {                                                                                          \
                current = current->next;                                                               \
            }                                                                                          \
            current->next = (element_##type *)malloc(sizeof(element_##type));                          \
            current->next->val = value;                                                                \
            current->next->next = NULL;                                                                \
        }                                                                                              \
    }                                                                                                  \
                                                                                                       \
    void _prepend_##type(element_##type **head, type value)                                            \
    {                                                                                                  \
        element_##type *current = (element_##type *)malloc(sizeof(element_##type));                    \
        current->val = value;                                                                          \
        current->next = *head;                                                                         \
        *head = current;                                                                               \
    }                                                                                                  \
                                                                                                       \
    int _remove_last_##type(element_##type *head)                                                      \
    {                                                                                                  \
        int retval = 0;                                                                                \
        /* if there is only one item in the list, remove it */                                         \
        if (head->next == NULL)                                                                        \
        {                                                                                              \
            retval = head->val;                                                                        \
            free(head);                                                                                \
            return retval;                                                                             \
        }                                                                                              \
                                                                                                       \
        /* get to the second to last node in the list */                                               \
        element_##type *current = head;                                                                \
        while (current->next->next != NULL)                                                            \
        {                                                                                              \
            current = current->next;                                                                   \
        }                                                                                              \
                                                                                                       \
        /* now current points to the second to last item of the list, so let's remove current->next */ \
        retval = current->next->val;                                                                   \
        free(current->next);                                                                           \
        current->next = NULL;                                                                          \
        return retval;                                                                                 \
    }                                                                                                  \
                                                                                                       \
    int _remove_by_index_##type(element_##type **head, int n)                                          \
    {                                                                                                  \
        int i = 0;                                                                                     \
        element_##type *current = *head;                                                               \
        element_##type *temp = NULL;                                                                   \
        if (n == 0)                                                                                    \
        {                                                                                              \
            return _remove_first_##type(head);                                                         \
        }                                                                                              \
                                                                                                       \
        for (i; i < n - 1; i++)                                                                        \
        {                                                                                              \
            if (current->next == NULL)                                                                 \
            {                                                                                          \
                return -1;                                                                             \
            }                                                                                          \
            current = current->next;                                                                   \
        }                                                                                              \
        if (current->next == NULL)                                                                     \
        {                                                                                              \
            return -1;                                                                                 \
        }                                                                                              \
        temp = current->next;                                                                          \
        current->next = temp->next;                                                                    \
        free(temp);                                                                                    \
        return 0;                                                                                      \
    }                                                                                                  \
                                                                                                       \
    int _remove_first_##type(element_##type **head)                                                    \
    {                                                                                                  \
        int retval = -1;                                                                               \
        element_##type *next = NULL;                                                                   \
                                                                                                       \
        if (*head == NULL)                                                                             \
        {                                                                                              \
            return retval;                                                                             \
        }                                                                                              \
                                                                                                       \
        next = (*head)->next;                                                                          \
        free(*head);                                                                                   \
        *head = next;                                                                                  \
        return 0;                                                                                      \
    }                                                                                                  \
    _list_functions_##type _list_func_##type = {                                                       \
        &_append_##type,                                                                               \
        &_prepend_##type,                                                                              \
        &_remove_last_##type,                                                                          \
        &_remove_first_##type,                                                                         \
        &_remove_by_index_##type,                                                                      \
    };\

#define element(type) element_##type *
#define create_list(type) _create_list_##type()

#endif