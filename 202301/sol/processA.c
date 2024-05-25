#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define ERR_ARGS 1

int sig1 = 0, sig2 = 0;
pid_t sig1_pid = 0, sig2_pid = 0;

void handler(int signu, siginfo_t * info, void * context) {
    if (signu == SIGUSR1) {
        sig1 = 1;
        sig1_pid = info->si_pid;
    }
    if (signu == SIGUSR2) {
        sig2 = 1;
        sig2_pid = info->si_pid;
    } 
}

int main(int argc, char * argv[]) {
    if (argc != 2) {
        printf("Usage: %s <pid>\n", argv[0]);
        exit(ERR_ARGS);
    }

    int pid = atoi(argv[1]);

    struct sigaction sa;
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
    }
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction");
    }
    int val = 0;
    while(1) {

        scanf("%d",  &val);
        if (val == 1) {
            printf("Sending SIGUSR1 to %d\n", pid);
            kill(pid, SIGUSR1);
        } else if (val == 2) {
            printf("Sending SIGUSR2 to %d\n", pid);
            kill(pid, SIGUSR2);
        }

        if (sig1) {
            printf("Recived SIGUSR1 from %d\n", sig1_pid);
            sig1 = 0;
        }
        if (sig2) {
            printf("Recived SIGUSR2 from %d\n", sig2_pid);
            sig2 = 0;
        }
        pause();
    }
}