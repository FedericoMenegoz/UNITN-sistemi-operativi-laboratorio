#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ERR_ARGS 1

#define MAX_BUFFER 256

#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define DF "\033[0m"

// flags
int sig1 = 0, sig2 = 0, transfer_queue = 0, terminate = 0;
// pids
int sig1_pid, sig2_pid;
// queue
int key, queueId;
struct msg_buf {
    long type;
    char mtext[MAX_BUFFER];
} msg_snd;

void workers_handler(int signu, siginfo_t *info,
                     __attribute__((unused)) void *ucontext) {
    if (signu == SIGUSR1) {
        sig1 = 1;
        sig1_pid = info->si_pid;
    }
    if (signu == SIGUSR2) {
        sig2 = 1;
        sig2_pid = info->si_pid;
    }
    if (signu == SIGINT) {
        transfer_queue = 1;
    }
    if (signu == SIGTERM) {
        terminate = 1;
    }
}

void master_handler(int signu) {
    if (signu == SIGWINCH) {
        transfer_queue = 1;
    }
    if (signu == SIGTERM) {
        terminate = 1;
    }
}

int main(int argc, char *argv[]) {
    int n;
    FILE *fd;
    pid_t pid;
    // Arguments checks.
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <n> </path/to/file.txt> <pid>\n", argv[0]);
        exit(ERR_ARGS);
    } else {
        // Checking n
        n = atoi(argv[1]);
        if (strspn(argv[1], "0123456789") != strlen(argv[1]) || n < 1 ||
            n > 10) {
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
        if ((fd = fopen(argv[2], "r")) == NULL) {
            fprintf(stderr, "File path does not exist.\n");
            exit(ERR_ARGS);
        }
    }
    int master_pid = getpid();
    printf("%s[%d] MASTER%s\n", GREEN, master_pid, DF);
    pid_t *workers = malloc(n * sizeof(pid_t));
    pid_t fork_pid;

    // Create queue
    key = ftok(argv[2], master_pid);
    if (key == -1) {
        perror("ftok");
    }
    queueId = msgget(key, IPC_CREAT | 0777);
    if (queueId == -1) {
        perror("msgget");
    }
    // Setting master as a group leader
    if (setpgid(0, 0) == -1) {
        perror("setpgid");
        exit(1);
    }
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
        // If child add the signal handler send the signal to pid and break the
        // loop
        if (fork_pid == 0) {
            sigaction(SIGUSR1, &act1, NULL);
            sigaction(SIGUSR2, &act2, NULL);
            sigaction(SIGINT, &act2, NULL);
            sigaction(SIGTERM, &act2, NULL);
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
            printf("%s[%d] Received SIGUSR1 from %d%s\n", BLUE, getpid(),
                   sig1_pid, DF);
            kill(sig1_pid, SIGUSR1);
            sig1 = 0;
        }
        if (sig2) {
            printf("%s[%d] Received SIGUSR2 from %d%s\n", BLUE, getpid(),
                   sig2_pid, DF);
            kill(sig2_pid, SIGUSR2);
            sig2 = 0;
        }
        if (transfer_queue) {
            fgets(msg_snd.mtext, MAX_BUFFER, fd);
            if (feof(fd)) {
                kill(master_pid, SIGTERM);
            } else {
                // Important to flush
                fflush(fd);
                int len = strlen(msg_snd.mtext) - 1;
                // fgets write also new line
                msg_snd.mtext[len] = 0;
                msg_snd.type = getpid();
                printf("%s[%d] Just read %s %s\n", BLUE, getpid(),
                       msg_snd.mtext, DF);

                if (msgsnd(queueId, &msg_snd, sizeof(msg_snd.mtext), 0) == -1) {
                    perror("msgsnd");
                }
            }

            transfer_queue = 0;
        }
        if (terminate) {
            printf("%s[%d] Child terminating...%s\n", BLUE, getpid(), DF);
            break;
        }
        pause();
    }
    if (fork_pid) {
        signal(SIGWINCH, master_handler);
        signal(SIGTERM, master_handler);
    }
    // Master process
    while (fork_pid) {
        if (transfer_queue) {
            int i = 0;
            while (!terminate) {
                i = (i + 1) % n;
                kill(workers[i], SIGINT);
                sleep(1);
            }
            printf("%s[%d]Terminating children.%s\n", GREEN, getpid(), DF);
            kill(-master_pid, SIGTERM);
            break;
        }
        if (terminate) {
            printf("%s[%d] Killing all children%s\n", GREEN, getpid(), DF);
            if (kill(-getpid(), SIGTERM) == -1) {
                perror("kill");
            }
            while (wait(NULL) > 0);
            break;
        }
        pause();
    }
    if (fork_pid) {
        sleep(1);
        msgctl(queueId, IPC_RMID, NULL);
        printf("%s[%d] PostOffice is closed ...%s\n", GREEN, getpid(), DF);
    }
    return 0;
}
