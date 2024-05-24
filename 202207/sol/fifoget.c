#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "lib.c"

int main(int argc, char* argv[]) {
    int fd;
    int err;
    int n = check_parse_args(argc, argv, &err, &fd);

    if (n < 0) {
        exit(err);
    }

    return 0;
}
