// #include "vector.h"
#include "c-lib/vector1.h"
#include "c-lib/str.h"
#include <stdio.h>

// define_list(int)
// define_list(double)

int main()
{
    vector vec = vec_new(sizeof(int));

    for (int i = 0; i < 160; i++)
    {
        append(&vec, &i);
    }
    for (int i = 0; i < vec.length; i++)
    {

        printf("%d ", at(&vec, i));
        printf((i + 1) % 5 ? "" : "\n");
    }

    // vector(double) List = new_vector(double);
    // printf("prepend:\n");
    // for (int i = 0; i < 160; i++){
    //     List.prepend(&List, i);
    // }
    // // _remove(&List, 3);
    // // _remove(&List, 4);
    // for (int i = 0; i < List.length; i++){

    //     printf("%f ", List.data[i]);
    //     printf((i + 1) % 5 ? "" : "\n");
    // }
    // printf("\nappend:\n");
    // List.reset(&List);
    // for (int i = 0; i < 160; i++){
    //     List.append(&List, i);
    // }
    // List.remove(&List, 3);
    // for (int i = 0; i < List.length; i++){

    //     printf("%f ", List.data[i]);
    //     printf((i + 1) % 5 ? "" : "\n");
    // }

    // List.destroy(&List);
    // printf("%s\n", substr("Hello World!", 1, 6));
}