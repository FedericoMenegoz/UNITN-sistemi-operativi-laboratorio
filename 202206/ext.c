#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define DF "\033[0m"
#define BOLD "\e[1m"
#define UNBOLD "\e[0m"

int fail = 0, success = 0, pid = 0;
void handler(int signu, siginfo_t * info, void *) {
    pid = info->si_pid;
    if (signu == SIGUSR1) {
        success = 1;
    } else if (signu == SIGUSR2) {
        fail = 1;
    }
}

int main() {
    struct sigaction sa;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    while(1) {
        printf("\n[%d]%s... WAITING MESSAGES ...%s\n\n", getpid(), BOLD, UNBOLD);
        if(fail) {
            printf("[%d] Process %d %s%sfailed%s%s\n", getpid(), pid, RED, BOLD, UNBOLD, DF);
            fail = 0;
        }
        if (success) {
            printf("[%d] Process %d %s%ssucceded%s%s\n", getpid(), pid, GREEN, BOLD, UNBOLD, DF);
            success = 0;
        }
        pause();
    }
}