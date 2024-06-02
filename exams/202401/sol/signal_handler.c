#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define MAX_CHILDREN 10

pid_t children[MAX_CHILDREN];

int main(void) {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_flags = SA_SIGINFO
    act.sa_sigaction = handler

    return 0;
}
