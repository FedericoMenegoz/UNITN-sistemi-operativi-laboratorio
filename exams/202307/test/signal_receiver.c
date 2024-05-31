#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define YELLOW "\033[0;33m"
#define DF "\033[0m"

int sig1 = 0, sig2 = 0, sig_term = 0, sig_int = 0, pid_sigint, pid_sig1 = 0,
    pid_sig2 = 0, pid_sigt = 0;
void handler(int signu, siginfo_t *info,
             __attribute__((unused)) void *ucontext) {
    if (signu == SIGUSR1) {
        sig1 = 1;
        pid_sig1 = info->si_pid;
    }
    if (signu == SIGUSR2) {
        sig2 = 1;
        pid_sig2 = info->si_pid;
    }
    if (signu == SIGTERM) {
        sig_term = 1;
        pid_sigt = info->si_pid;
    }
    if (signu == SIGINT) {
        sig_int = 1;
        pid_sigint = info->si_pid;
    }
}

int main(int argc, char *argv[]) {
    int count = 0;
    if (argc == 2) {
        count = atoi(argv[1]);
    }
    printf("%s[%d] Signal Receiver%s\n", YELLOW, getpid(), DF);

    struct sigaction sa;
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    while (1) {
        if (sig1) {
            printf("%s[%d] Received SIGUSR1 from %d.%s\n", YELLOW, getpid(),
                   pid_sig1, DF);
            kill(pid_sig1, SIGUSR1);
            sig1 = 0;
        }
        if (sig2) {
            printf("%s[%d] Received SIGUSR2 from %d.%s\n", YELLOW, getpid(),
                   pid_sig2, DF);
            kill(pid_sig2, SIGUSR1);
            sig2 = 0;
        }
        if (sig_term) {
            count--;
            printf("%s[%d] Received SIGTERM from %d. %s\n", YELLOW, getpid(),
                   pid_sigt, DF);
            sig_term = 0;
        }
        if (sig_int) {
            printf("%s[%d] TERMINATING ...%s\n", YELLOW, getpid(), DF);
            break;
        }
        pause();
    }
    if (count) {
        return 1;
    } else {
        return 0;
    }
}
