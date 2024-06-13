#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>


#define ERR_ARGS 3
#define ERR_FIFO 2
#define INCOMPLETE 1
#define COMPLETE 0
#define MAX_MSG 10


int main(int argc, char * argv[]) {
    int n;
    char buffer[MAX_MSG];
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <path> <n>.\n", argv[0]);
        return ERR_ARGS;
    } else {
        n = atoi(argv[2]);
        if (strspn(argv[2], "0123456789") != strlen(argv[2]) || n < 0 || n > 10) {
            fprintf(stderr, "n must be a valid int in between 0 and 10.\n");
            return ERR_ARGS;
        }
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return ERR_FIFO;
    }
    int i = 0, err = 0;
    for (; i < n && !err; i++) {
        int bytes = read(fd, &buffer[i], sizeof(char));
        if(bytes == -1) {
            perror("read");
            close(fd);
            return ERR_FIFO;
        } else if (bytes == 0) {
            err = 1;
            i--;
        }
    }
    i--;
    for (; i >= 0; i--){
        printf("%c\n", buffer[i]);
    }
    close(fd);
    return err;

}
