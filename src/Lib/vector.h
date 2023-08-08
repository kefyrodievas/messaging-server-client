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

typedef struct{

} _vector_functions;

int _append(_vector *vector, int element){
    int *new;
    // _vector new_v;
    if(vector->length + 1 > vector->size){
        new = (int*)malloc(vector->size * 2 * sizeof(int));
        memcpy(new, vector->data, vector->size * sizeof(int));

        free(vector->data);
        vector->data = new;
        vector->size *= 2;
    }
    vector->data[vector->length] = element;
    vector->length++;
    
}

void _prepend(struct _vector *vector, int element){
    vector->size = vector->length + 1 > vector->size ? vector->size *=2 : vector->size;
    // if(vector->length + 1 > vector->size){
    //     // new = (int*)malloc(vector->size * 2 * sizeof(int));
    //     vector->size *=2;
    // // }
    int *new = (int*)malloc(vector->size * sizeof(int));

    // // int * new;
    // // new + 1 = vector->data;
    
    memcpy(new + 1, vector->data, vector->length * sizeof(int));
    // int * new = vector->data;
    // int s = sizeof(int);
    // *(int*)new <<= 7;
    new[0] = element;
    free(vector->data);
    vector->data = new;
    vector->length++;
}

void _remove(struct _vector *vector, int pos){
    int * new = malloc(vector->size * sizeof(int));
    int * data = (*vector).data;
    memcpy(new, data, (pos - 1) * sizeof(int));
    memcpy(new + pos - 1, data + pos, (vector->size - pos + 1) * sizeof(int));
    free(data);
    // free(vector->data);
    vector->data = new;
    vector->length -= 1;
}

void _destroy(struct _vector *vector){
    free(vector->data);
    vector->length = NULL;
    vector->size = NULL;
}

void _reset(struct _vector *vector){
    free(vector->data);
    *vector = new_vector();
}

#endif