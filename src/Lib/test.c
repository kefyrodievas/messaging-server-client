#include "vector.h"
#include "str.h"
#include <stdio.h>


int main(){
    struct _vector List = new_vector();
    for(int i = 0; i < 11; i++){
        prepend(&List, i);
        
    }
    for(int i = 0; i < 11; i++){
        printf("%d\n", List.data[i]);
    }
    // printf("%d\n", List.data[9]);
    
}