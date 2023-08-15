#include "c-lib/vector.h"
// #include "c-lib/vector1.h"
#include "c-lib/str.h"
#include <stdio.h>

define_list(int) define_list(double)

    int main() {
  // vector vec = vec_new(sizeof(int));

  // for (int i = 0; i < 160; i++)
  // {
  //     append(&vec, &i);
  // }
  // for (int i = 0; i < vec.length; i++)
  // {

  //     printf("%d ", at(&vec, i));
  //     printf((i + 1) % 5 ? "" : "\n");
  // }

  vector(int) List = new_vector(int);
  printf("prepend:\n");
  for (int i = 0; i < 161; i++) {
    List.prepend(&List, i);
  }
  List.shrink(&List);
  // _remove(&List, 3);
  // _remove(&List, 4);
  for (int i = 0; i < List.size; i++) {
    printf("%d ", List.data[i]);
    printf((i + 1) % 5 ? "" : "\n");
  }
  printf("\nappend:\n");
  List.reset(&List);
  for (int i = 0; i < 161; i++) {
    List.append(&List, i);
  }
  List.shrink(&List);
  // List.remove(&List, 3);
  for (int i = 0; i < List.size; i++) {

    printf("%d ", List.data[i]);
    printf((i + 1) % 5 ? "" : "\n");
  }

  List.destroy(&List);
}

void test() {}