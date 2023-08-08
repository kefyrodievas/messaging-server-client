#include "vector.h"
#include "str.h"
#include <stdio.h>

int main()
{
    struct _vector List = new_vector();
    printf("prepend:\n");
    for (int i = 0; i < 160; i++){
        _prepend(&List, i);
    }
    // _remove(&List, 3);
    // _remove(&List, 4);
    for (int i = 0; i < List.length; i++){

        printf("%d ", List.data[i]);
        printf((i + 1) % 5 ? "" : "\n");
    }
    printf("\nappend:\n");
    _reset(&List);
    for (int i = 0; i < 160; i++){
        _append(&List, i);
    }
    for (int i = 0; i < List.length; i++){

        printf("%d ", List.data[i]);
        printf((i + 1) % 5 ? "" : "\n");
    }
    _destroy(&List);
}