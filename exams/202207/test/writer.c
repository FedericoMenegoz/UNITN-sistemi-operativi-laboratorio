#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>

#define RED "\033[0;31m"
#define DF "\033[0m"

#define ERR_ARGS 1
#define ERR_FIFO 2

#define ARGS 2
#define MAX_BUFFER 10
#define MSG_SIZE 1

char buffer[MAX_BUFFER];

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("%s[Writer] Usage: %s <fifo_path> <string>%s\n", RED, argv[0],
               DF);
        exit(ERR_ARGS);
    } else if (strspn(argv[2], "0123456789ABCDEFGHIJKLMNOPQRSTUVXYWZ") !=
                   strlen(argv[2]) ||
               strlen(argv[2]) > 10) {
        printf(
            "%s[Witer] Input string not longer than 10 and valid character are "
            "[0-9 | A-Z]%s\n",
            RED, DF);
        exit(ERR_ARGS);
    }

    int n = strlen(argv[2]);
    remove(argv[1]);
    if (mkfifo(argv[1], O_CREAT | 0777) == -1) {
        perror("[Writer] mkfifo");
        return ERR_FIFO;
    }
    int fd = open(argv[1], O_WRONLY);
    if (fd == -1) {
        perror("[Writer] open");
    }
    char c;

    for (int i = 0; i < n; i++) {
        c = argv[2][i];
        if (write(fd, &c, sizeof(char)) == -1) {
            perror("[Writer] Error writing... ");
        }
    }

    close(fd);
    return 0;
}
