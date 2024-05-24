#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "../sol/lib.c"


char buffer[MAX_BUFFER];

int main(int argc, char * argv[]) {
    int err;
    
    if (argc != 3) {
        printf("%s[Writer] Usage: %s <fifo_path> <string>%s\n", RED, argv[0], DF);
        exit(ERR_ARGS);
    } else if (strspn(argv[2], "0123456789ABCDEFGHIJKLMNOPQRSTUVXYWZ") != strlen(argv[2]) || strlen(argv[2]) > 10) {
        printf("%s[Witer] Input string not longer than 10 and valid character are [0-9 | A-Z]%s\n", RED, DF);
        exit(ERR_ARGS);
    }
    
    int n = strlen(argv[2]);
    int fd = open_fifo(argv[1], &err, O_WRONLY);
    if (fd == -1) {
        exit(err);
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