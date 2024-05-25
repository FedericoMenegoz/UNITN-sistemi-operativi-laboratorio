#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define ERR_ARGS 1


#define MAX_LOG 16 // # <pid:8> -# <signu:2> 

struct msgq {
    long type;
    char text[MAX_LOG];
} msg_snd;

int sig1 = 0, sig2 = 0, sigalrm = 0;
pid_t sig1_pid = 0, sig2_pid = 0, al_pid = 0;

void handler(int signu, siginfo_t * info, void * context) {
    if (signu == SIGUSR1) {
        sig1 = 1;
        sig1_pid = info->si_pid;
    }
    if (signu == SIGUSR2) {
        sig2 = 1;
        sig2_pid = info->si_pid;
    }
    if (signu == SIGALRM) {
        sigalrm = 1;
        al_pid = info->si_pid;
    }
}

int main(int argc, char * argv[]) {
    if (argc != 3) {
        printf("Usage: %s <logFile> <pid>\n", argv[0]);
        exit(ERR_ARGS);
    }
    printf("[%d] TEST \n", getpid());
    int pid = atoi(argv[2]);

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

    int key = ftok(argv[1], 1);
    if (key == -1) {
        perror("ftok");
    }
    int queueId =  msgget(key, IPC_CREAT | 0666);

    if ( queueId == -1) {
        perror("msgget");
    }
    int val = 0;
    printf("1-2 (SIGUSR1-2 to %d)\n", pid);
    printf("3 (write #%d to mq)\n", getpid());
    printf("4 kill %d\n", pid);
    while(1) {
        printf("Input: ");
        fflush(stdout);
        scanf("%d", &val);
        
        if (val == 1) {
            printf("Sending SIGUSR1 to %d\n", pid);
            kill(pid, SIGUSR1);

        } else if (val == 2) {
            printf("Sending SIGUSR2 to %d\n", pid);
            kill(pid, SIGUSR2);

        } else if (val == 3) {
            snprintf(msg_snd.text, MAX_LOG - 1, "#%d", getpid());
            msg_snd.type = 1;
            if (msgsnd(queueId, &msg_snd, strlen(msg_snd.text) + 1, 0) == -1) {
                perror("msgsnd");
            }
            kill(pid, SIGCONT);
        } else if (val == 4) {
            kill(pid, SIGTERM);
        } else {
            break;
        }

        if (sig1) {
            printf("[%d] Recived SIGUSR1 from %d\n", getpid(), sig1_pid);
            sig1 = 0;
        }
        if (sig2) {
            printf("Recived SIGUSR2 from %d\n", sig2_pid);
            sig2 = 0;
        }
        if (sigalrm) {
            printf("Received SIGALARM from %d\n", al_pid);
        }
        sleep(1);
    }
}