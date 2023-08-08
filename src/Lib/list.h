#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    int value;
    struct _list_elem * next;
} _list_elem;

typedef struct {

} _list_elem_functions;

typedef struct _list{
    size_t size;
    _list_elem data;
    _list_elem_functions _list_elem_funcs;
} List;



#endif