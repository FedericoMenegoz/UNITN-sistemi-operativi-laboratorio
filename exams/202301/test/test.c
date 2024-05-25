#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ERR_ARGS 1

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define DF "\033[0m"
#define BOLD "\033[1m"
#define UNBOLD "\033[0m"

#define MAX_LOG 16  // # <pid:8> -# <signu:2>
#define SIGNAL_PROXY_BIN "./sol/SignalProxy.out"

void success(char *msg) {
    printf("%s%s%s%s SUCCESS ! %s%s\n", BLUE, msg, GREEN, BOLD, UNBOLD, DF);
}

void fail(char *msg) {
    printf("%s%s%s%s FAILURE !%s%s\n", BLUE, msg, RED, BOLD, UNBOLD, DF);
}

void check_condition(int condition, char *msg) {
    if (condition) {
        success(msg);
    } else {
        fail(msg);
    }
}

struct msgq {
    long type;
    char text[MAX_LOG];
} msg_snd;

int sig1 = 0, sig2 = 0, sigalrm = 0;
pid_t sig1_pid = 0, sig2_pid = 0, al_pid = 0;

void handler(int signu, siginfo_t *info,
             __attribute__((unused)) void *context) {
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

int main(int argc, char *argv[]) {
    char path_to_log[32] = "sol/log.txt";
    // Check args
    if (argc == 2) {
        // Check if logfile file exists
        if (access(argv[1], F_OK) != 0) {
            printf("File %s not found. Call 'make FILE=%s'\n", argv[1], argv[1]);
            exit(ERR_ARGS);
        }
       strcpy(path_to_log, argv[1]);
    }

    int signal_pid = fork();
    if (signal_pid == -1) {
        perror("fork");
        exit(2);
    }
    // Child run SignalProxy.out
    if (signal_pid == 0) {
        printf("[%d] Child to exec SignalProxy\n", getpid());
        if (execl(SIGNAL_PROXY_BIN, SIGNAL_PROXY_BIN, path_to_log, NULL) == -1) {
            perror("execl");
            exit(2);
        }
    }

    sleep(1);

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
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
    }

    printf("%s%sChecking SIGUSR1%s%s\n", YELLOW, BOLD, UNBOLD, DF);
    // Send SIGUSR1 to SignalProxy
    kill(signal_pid, SIGUSR1);
    sleep(1);

    // Check return signal
    check_condition(sig1 == 1 && sig1_pid == signal_pid, "SIGUSR1");
    
    printf("%s%sChecking SIGUSR2%s%s\n", YELLOW, BOLD, UNBOLD, DF);
    // Send SIGUSR" to SignalProxy
    kill(signal_pid, SIGUSR2);
    sleep(1);

    // Check return signal
    check_condition(sig2 == 1 && sig2_pid != signal_pid, "SIGUSR2");

    printf("%s%sChecking message queue.%s%s\n", YELLOW, BOLD, UNBOLD, DF);
    // Send message to the queue
    int key = ftok(path_to_log, 1);
    int queueId = msgget(key, IPC_CREAT | 0666);
    if (queueId == -1) {
        perror("msgget");
    }

    struct msg {
        long type;
        char text[MAX_LOG];
    } msg_rcv;

    msg_rcv.type = 1;
    snprintf(msg_rcv.text, MAX_LOG, "#%d", getpid());
    if (msgsnd(queueId, &msg_rcv, strlen(msg_rcv.text), 0) == -1) {
        perror("msgsnd");
    }
    sleep(1);
    // Check for SIGALARM
    check_condition(sigalrm == 1 && al_pid == signal_pid, "SIGALARM");
    // Wait signal to write everything on log file
    sleep(5);
    // Simulate a ctrl-c
    kill(signal_pid, SIGINT);
    sleep(3);
    // Terminate SignalProxy
    kill(signal_pid, SIGTERM);
    wait(NULL);
    printf("%s%sChecking %s ...%s%s\n",YELLOW, BOLD, path_to_log, UNBOLD, DF);
    // Check for file log
    FILE *f = fopen(path_to_log, "r");
    char buffer[MAX_LOG];
    fscanf(f, "%5s\n", buffer);
    check_condition(strcmp(buffer, "start") == 0, "start=start");
    int pid_to_check, sig_to_check;
    fscanf(f, "#%d-#%d\n", &pid_to_check, &sig_to_check);
    check_condition(pid_to_check == getpid() && sig_to_check == SIGUSR1,
                    "first signal");
    fscanf(f, "#%d-#%d\n", &pid_to_check, &sig_to_check);
    check_condition(pid_to_check == getpid() && sig_to_check == SIGUSR2,
                    "second signal");
    fscanf(f, "%4s\n", buffer);
    check_condition(strcmp(buffer, "stop") == 0, "stop=stop");
    fclose(f);
    return 0;
}
