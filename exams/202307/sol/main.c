#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define ERR_ARGS 1

#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define DF "\033[0m"

// flags
int sig1 = 0, sig2 = 0;
// pids
int sig1_pid, sig2_pid;

void workers_handler(int signu, siginfo_t *info,__attribute__((unused)) void *ucontext) {
    if(signu == SIGUSR1) {
        sig1 = 1;
        sig1_pid = info->si_pid;
    }
    if (signu == SIGUSR2) {
        sig2 = 1;
        sig2_pid = info->si_pid;
    }
}

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

    printf("%s[%d] MASTER%s\n", GREEN, getpid(), DF);
    pid_t * workers = malloc(n*sizeof(pid_t));
    pid_t fork_pid;

    // // Make the master ignore SIGUSR1 SIGUSR2
    // signal(SIGUSR1, SIG_IGN);
    // signal(SIGUSR2, SIG_IGN);
    
    // Creating sigaction for workers
    struct sigaction act1;
    act1.sa_sigaction = workers_handler;
    act1.sa_flags = SA_SIGINFO | SA_RESETHAND;
    sigemptyset(&act1.sa_mask);

    struct sigaction act2;
    act2.sa_flags = SA_SIGINFO;
    act2.sa_sigaction = workers_handler;
    sigemptyset(&act2.sa_mask);

    // Workers creation
    for (int i = 0; i < n; i++) {
        fork_pid = fork();
        // If child add the signal handler send the signal to pid and break the loop
        if (fork_pid == 0) {
            sigaction(SIGUSR1, &act1, NULL);
            sigaction(SIGUSR2, &act2, NULL);
            printf("%s[%d] WORKER%s\n", BLUE, getpid(), DF);
            kill(pid, SIGTERM);
            break; 
        } 
        // Save child pid into workers
        else {
            workers[i] = fork_pid;
            // To avoid all the SIGTERM to collide together
            usleep(50000);
        }
    }

    // Worker process
    while (!fork_pid) {
        if (sig1) {
            printf("%s[%d] Received SIGUSR1 from %d%s\n", BLUE, getpid(), sig1_pid, DF);
            kill(sig1_pid, SIGUSR1);
            sig1 = 0;
        }
        if (sig2) {
            printf("%s[%d] Received SIGUSR2 from %d%s\n", BLUE, getpid(), sig2_pid, DF);
            kill(sig2_pid, SIGUSR2);
            sig2 = 0;
        }
        pause();
    }
    // Master process
    while(fork_pid) {
        pause();
    }
    return 0;
}