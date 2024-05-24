#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>

#define ERR_ARGS 1
#define ERR_FIFO 2

#define ARGS 2
#define MAX_BUFFER 10
#define MSG_SIZE 1

#define RED "\033[0;31m"
#define DF "\033[0m"

char buffer[MAX_BUFFER];

int get_n(char* str) {
    int ret = atoi(str);
    if (strspn(str, "0123456789") == strlen(str) && ret >= 0 &&
        ret <= MAX_BUFFER) {
        return ret;
    }
    return -1;
}

int check_parse_args(int argc, char* argv[], int * err) {
    if (argc != 3) {
        printf("%sUsage: %s <path> <n>.%s\n", RED, argv[0], DF);
        *err = ERR_ARGS;
        return -1;
    }
    int n = get_n(argv[2]);
    if (n == -1) {
        printf("%sn must be an integer from 0 to 10 included.%s\n", RED, DF);
        *err = ERR_ARGS;
        return -1;
    }
    
    return n;
}

int open_fifo(const char * path, int * err, int flag) {
    int fd;
    if ((fd = open(path, flag)) == -1) {
        printf("%s", RED);fflush(stdout);
        perror("Error opening the fifo");
        printf("%s", DF); fflush(stdout);
        *err = ERR_FIFO;
        return -1;
    }
    return fd;
}

int fifo_to_buffer(int fd, int * n) {
    int err = 0;
    for (int i = 0; i < *n && !err; i++) {
        char c;
        int bytes = read(fd, &c, sizeof(char));
        if ( bytes == -1) {
            perror("[Reader] Error reading...");
            err = 1; 
        } 
        else if (bytes == 0) {
            err = 1;
            *n = i;
        }
        else {
            buffer[i] = c;
        }
    }
    return err;
}
int convert_from_char(char c) {
    return c - '0';
}