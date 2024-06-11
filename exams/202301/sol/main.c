#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ERR_ARGS 1
#define ERR_FILE 2
#define MAX_THREADS 10

#define MAX_LOG 16  //  <pid:8> - <signu:2>

struct msgq {
    long type;
    char text[MAX_LOG];
} msg_rcv;

int sig1 = 0, sig2 = 0, ctrlC = 0, terminate = 0;
pid_t sig1_pid = 0, sig2_pid = 0;

pthread_t thread;

int fd_log = 0;

static void* thread_log(void* arg) {
    sleep(3);
    char* message = (char*)arg;
    if (write(fd_log, message, strlen(message)) == -1) {
        perror("writing");
    }
    free(message);
    return NULL;
}

void write_to_log(pid_t sig_pid, int signo);

void handler(int signu, siginfo_t* info, void* context) {
    if (signu == SIGUSR1) {
        sig1 = 1;
        sig1_pid = info->si_pid;
    }
    if (signu == SIGUSR2) {
        sig2 = 1;
        sig2_pid = info->si_pid;
    }
    if (signu == SIGINT) {
        ctrlC = 1;
    }
    if (signu == SIGTERM) {
        terminate = 1;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <pathToLogFile>\n", argv[0]);
        exit(ERR_ARGS);
    }
    printf("[%d]\n", getpid());

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
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
    }

    fd_log = open(argv[1], O_WRONLY | O_APPEND, 0666);

    if (fd_log == -1) {
        perror("Open");
        exit(ERR_FILE);
    }

    int key = ftok(argv[1], 1);

    if (key == -1) {
        perror("ftok");
    }

    int queueId = msgget(key, IPC_CREAT | 0666);

    if (queueId == -1) {
        perror("msgget");
    }

    while (1) {
        if (sig1) {
            printf("Received sig1 from %d\n", sig1_pid);
            kill(sig1_pid, SIGUSR1);
            write_to_log(sig1_pid, SIGUSR1);
            sig1 = 0;
        }
        if (sig2) {
            printf("Received sig2 from %d\n", sig2_pid);
            if (fork() == 0) {
                printf("[%d] Child...\n", getpid());
                kill(sig2_pid, SIGUSR2);
                break;
            }
            write_to_log(sig2_pid, SIGUSR2);
            sig2 = 0;
        }
        if (ctrlC) {
            if (write(fd_log, "stop\n", strlen("stop\n")) == -1) {
                perror("writing stop");
            }
            ctrlC = 0;
        }
        int bytes =
            msgrcv(queueId, &msg_rcv, sizeof(msg_rcv.text), 0, IPC_NOWAIT);
        if (bytes == -1) {
            if (errno != ENOMSG) {
                perror("msgrcv");
            }
        } else if (bytes > 0) {
            pid_t pid = atoi(msg_rcv.text);
            kill(pid, SIGALRM);
        }
        if (terminate) {
            printf("[%d] ... TERMINATING ... \n", getpid());
            close(fd_log);
            if (msgctl(queueId, IPC_RMID, NULL) == -1) {
                perror("msgctl");
            }
            pthread_join(thread, NULL);
            break;
        }
    }
}

void write_to_log(pid_t sig_pid, int signo) {
    char* message = (char*)malloc(MAX_LOG * sizeof(char));
    snprintf(message, MAX_LOG, "%d-%d\n", sig_pid, signo);
    if (pthread_create(&thread, NULL, thread_log, message)) {
        perror("pthred");
    }
}
