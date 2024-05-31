#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define ERR_ARGS 1

int main(int argc, char * argv[]) {
    int n;
    pid_t pid;
    // Arguments checks.
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <n> </path/to/file.txt> <pid>\n", argv[0]);
        exit(ERR_ARGS);
    } else {
        // Checking n
        n = atoi(argv[1]);
        if (strspn(argv[1], "0123456789") != strlen(argv[1]) || n < 1 || n > 10) {
            fprintf(stderr, "n must be an int from 1 to 10 included\n");
            exit(ERR_ARGS);
        }
        // Checking pid is a valid number
        pid = atoi(argv[3]);
        if (strspn(argv[1], "0123456789") != strlen(argv[1])) {
            fprintf(stderr, "Pid must be a valid number.\n");
            exit(ERR_ARGS);
        }
        // Checking pid correspond to a running process
        if (kill(pid, 0) == -1) {
            fprintf(stderr, "No process with pid %d is running.\n", pid);
            exit(ERR_ARGS);
        }
        // Checking if path exists
        if (open(argv[2], 0) == -1) {
            fprintf(stderr, "File path does not exist.\n");
            exit(ERR_ARGS);
        }
    }
    return 0;
}