#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "lib.c"

char buffer[MAX_BUFFER];

int main(int argc, char* argv[]) {
    int err, fd;
    int n = check_parse_args(argc, argv, &err);

    if (n < 0) {
        exit(err);
    }
    
    fd = open_fifo(argv[1], &err, O_RDONLY);
    if (fd == -1) {
        exit(err);
    }

    err = fifo_to_buffer(fd, &n);

    for(int i = 0; i < n; i++) {
        printf("%c\n", buffer[i]);
    }

    close(fd);
    return err;
}
