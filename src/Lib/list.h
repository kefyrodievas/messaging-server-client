#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <string.h>

struct _vector{
    int * data;
    size_t size;
    size_t length;
};

struct _vector new_vector(){
    struct _vector ret;
    ret.data = (int *)malloc(10 * sizeof(int));
    ret.size = 10;
    ret.length = 0;
    return ret;
}

void append(struct _vector *vector, int element){
    int *new;
    if(vector->size == 0){
        *vector = new_vector();
    }
    if(vector->length + 1 > vector->size){
        new = (int*)malloc(vector->size * 2 * sizeof(int));
        memcpy(new, vector->data, vector->size);
        free(vector->data);
        vector->data = new;
    }
    vector->data[vector->length] = element;
}

void prepend(struct _vector *vector, int element){
    int *new;
    if(vector->size == 0){
        vector = new_vector;
    }
    if(vector->length + 1 > vector->size){
        new = (int*)malloc(vector->size * 2 * sizeof(int));
        memcpy(new + 1, vector->data, vector->size);
        free(vector->data);
        vector->data = new;
    }
    vector->data[0] = element;
}

#endif