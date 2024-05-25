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
    err = 1;
    char c;
    int count = 1;
    while (1) {
        int bytes = read(fd, &c, sizeof(char));

        if (bytes == -1) {
            perror("Error reading the fifo");
            break;
        } else if (bytes == 0) {
            break;
        } else if (count == n) {
            err = 0;
        } else if (count != n) {
            printf("%c\n", c);
        }
        count++;
    }

    close(fd);
    return err;
}
