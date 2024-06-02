#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define ERR_ARG 1
#define ERR_CHILD 2
#define ERR_PIPE 3
#define ERR_MASTER 4
#define MAX_N 10
#define MAX_BUFFER 256

int count_letter(char c, char * buffer) {
    if (*buffer == 0) {
        return 0;
    }
    if (*buffer == c || *buffer == toupper(c)) {
        return 1 + count_letter(c, buffer + 1);
    }
    return 0 + count_letter(c, buffer + 1);
}   


int main(int argc, char *argv[]) {
    int n; 
    FILE *file;
    char c;
    // Checking args
    if (argc != 4) {
        printf("Usage: %s <F> <N> <C>\n", argv[0]);
        exit(ERR_ARG);
    } 
    else {
        // Check if file exists
        if ((file = fopen(argv[1], "r")) == NULL) {
            printf("%s does not exist.\n", argv[1]);
            exit(ERR_ARG);
        }
        // Check if N is valid  
        n = atoi(argv[2]);
        if (strspn(argv[2], "0123456789") != strlen(argv[2]) || n < 1 || n > 10) {
            printf("N must be an integer from 1 to 10 included.\n");
            exit(ERR_ARG);
        }
        // Check if C is a valid char
        c = tolower(argv[3][0]);
        if (strlen(argv[3]) != 1 || c < 'a' || c > 'z') {
            printf("C must be a valid letter [a-zA-Z]\n");
            exit(ERR_ARG);
        }
    }

    // Count total letters in file
    int total_char = 0;
    while(fgetc(file) != EOF) {
        total_char++;
    }
    fclose(file);

    int fraction = total_char / n;
    int reminder = total_char % n;
    int fork_pid, offset, fd;
    int pfd[2];

    if (pipe(pfd) == -1) {
        perror("pipe");
        exit(ERR_PIPE);
    }
    // Create children
    for (int i = 0; i < n; i++) {
        fork_pid = fork();

        if (fork_pid == -1) {
            perror("fork");
            exit(ERR_CHILD);
        }
        
        if (fork_pid == 0) {
            // offset of ith child
            offset = i * fraction;
            fd = open(argv[1], O_RDONLY);
            if (fd == -1) {
                perror("open");
                exit(ERR_CHILD);
            }
            if (i == n-1) {
                fraction += reminder;
            }     
            break;
        }

    }
    
    if(!fork_pid) {
        // Close reading fd
        close(pfd[0]);
        size_t size = (fraction)*sizeof(char);
        char * buffer = malloc(size);
        // Move needle at the right offset
        if (lseek(fd, offset, SEEK_SET) == -1) {
            perror("lseek");
            free(buffer);
            exit(ERR_CHILD);
        }
        // Read file
        int bytes = read(fd, buffer, size);
        if ( bytes == -1 ) {
            free(buffer);
            perror("read");
            exit(ERR_CHILD);
        }
        // Add string terminator
        buffer[bytes] = 0;
        // Count occurences 
        int count = count_letter(c, buffer);
        // Format number + space to avoid merging two numbers from two different childs
    
        snprintf(buffer, 16, "%d ", count);
        // Write to pipe
        if (write(pfd[1], buffer, strlen(buffer)) == -1) {
            perror("");
        }
        
        // Close and exit
        close(pfd[1]);
        free(buffer);
    }
    // Parent Process
    if(fork_pid) {
        while(wait(NULL) > 0);
        // Close writing side of pipe
        close(pfd[1]);
        char buffer[MAX_BUFFER];
        int bytes = read(pfd[0], buffer, MAX_BUFFER);
        if (bytes == -1) {
            perror("read");
            exit(ERR_MASTER);
        }
        char *tkn = strtok(buffer, " ");
        int total = atoi(tkn);
        while((tkn = strtok(NULL, " ")) != NULL) {
            total += atoi(tkn);
        }

        printf("The total count of the letter %c is %d\n", c, total);
        close(pfd[0]);
    }
    return 0;
}
