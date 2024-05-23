#include <stdio.h>
#include <stdlib.h>

#define ERR_ARGS 2

int main(int argc, char * argv[]) {
    if (argc != 3) {
        printf("Usage: %s <path> <n>.\n", argv[0]);
        exit(ERR_ARGS);
    }

    return 0;
}