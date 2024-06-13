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
#define OPERATION_LEN 24 // 10 digits + 10 symbol + 2 (max could be 90)

int main(int argc, char * argv[]) {
    int n;
    char chars[MAX_MSG];
    int numbers[MAX_MSG];
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
    int chars_index = 0, err = 0, numbers_index = 0;
    while (chars_index + numbers_index < n && !err) {
        char c;
        int bytes = read(fd, &c, sizeof(char));
        if(bytes == -1) {
            perror("read");
            close(fd);
            return ERR_FIFO;
        } else if (bytes == 0) {
            err = 1;
        } else {
            if (c >= 'A' && c <= 'Z') {
                chars[chars_index++] = c;
            } else if (c >= '0' && c <= '9') {
                numbers[numbers_index++] = c - '0';
            } else {
                fprintf(stderr, "found %c which is not valid...", c);
            }
        }
    }

    for (int i = chars_index - 1; i >= 0; i--) {
        printf("%c\n", chars[i]);
    }

 
    int sum = 0;

    for (int i = numbers_index - 1; i >= 0; i--) {
        sum += numbers[i];
        printf("%d", numbers[i]);
        if (i > 0) printf("+"); else printf("=%d\n", sum);
    }

    close(fd);
    return err;

}
