#include <stdio.h>
#include <stdlib.h>

#define ERR_ARGS 1

int main(int argc, char * argv[]) {
    if (argc != 2) {
        printf("Usage: %s <pathToLogFile>\n", argv[0]);
        exit(ERR_ARGS);
    }


}