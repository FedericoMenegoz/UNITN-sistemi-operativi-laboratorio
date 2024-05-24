#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>

#define ERR_ARGS 1
#define ERR_FIFO 2

#define ARGS 2
#define MAX_N 10
#define MIN_N 0

#define RED "\033[0;31m"
#define DF "\033[0m"

int get_n(char* str) {
    int ret = atoi(str);
    if (strspn(str, "0123456789") == strlen(str) && ret >= MIN_N &&
        ret <= MAX_N) {
        return ret;
    }
    return -1;
}

int check_parse_args(int argc, char* argv[], int * err, int * fd) {
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
    if ((*fd = open(argv[1], O_RDONLY)) == -1) {
        printf("%s", RED);fflush(stdout);
        perror("Error opening the fifo");
        printf("%s", DF); fflush(stdout);
        *err = ERR_FIFO;
        return -1;
    }
    return n;
}

