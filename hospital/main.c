#include "../include/utils.h"
#include <stdio.h>


int main(int argc, char * argv[]) {
    if (argc != 2) {
        perr("Ciuao!");
    } 
    printf("%d\n%s\n", str_to_int("12"), argv[0]);
    return 0;
}