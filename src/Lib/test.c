#include "list.h"
#include "str.h"
#include <stdio.h>

define_list(int);

int main(){
    element(int) List = create_list(int);
    append(List, 1);
    printf("%d", List -> val);
}