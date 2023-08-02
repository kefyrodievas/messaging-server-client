#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>

typedef struct _vector{
    int * data;
    size_t size;
    size_t length;
}_vector;

_vector new_vector(){
    _vector ret;
    ret.data = (int *)malloc(10 * sizeof(int));
    ret.size = 10;
    ret.length = 0;
    return ret;
}

int append(_vector *vector, int element){
    int *new;
    // _vector new_v;
    if(vector->length + 1 > vector->size){
        new = (int*)malloc(vector->size * 2 * sizeof(int));
        memcpy(new, vector->data, vector->size * sizeof(int));
        free(vector->data);
        vector->data = new;
    }
    vector->data[vector->length] = element;
    vector->length++;
}

void prepend(struct _vector *vector, int element){
    int *new;
    // if(vector->size == 0){
    //     vector = new_vector();
    // }
    if(vector->length + 1 > vector->size){
        new = (int*)malloc(vector->size * 2 * sizeof(int));
        memcpy(new + 1, vector->data, vector->size * sizeof(int));
        free(vector->data);
        vector->data = new;
    }
    new = (int*)malloc(vector->size * 2 * sizeof(int));
    memcpy(new + 1, vector->data, vector->size * sizeof(int));
    vector->size *=2;
    new[0] = element;
    memcpy(vector->data, new, vector->size * sizeof(int));
}

#endif