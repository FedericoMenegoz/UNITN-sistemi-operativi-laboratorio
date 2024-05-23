#include <stdio.h>
#include <stdlib.h> // exit
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h> // O_CREAT
#include <unistd.h> // write
#include <errno.h>
#include <string.h>
#include <signal.h>

#define ERR_ARGS 2
#define ERR_PATH 3
#define ERR_FQ 4
#define ERR_MQ 5

#define RED "\033[0;31m"
#define DF "\033[0m"
#define BOLD "\e[1m"
#define UNBOLD "\e[0m"

#define MAX_BUFFER 96
#define PATH_MAX_LENGTH 60
#define CHILDREN_MAX 10
#define PIDLEN 8
#define FTOK_ID 32

char error_buffer[MAX_BUFFER];
char buffer[MAX_BUFFER];
char path[PATH_MAX_LENGTH];
int flag_file = 0, flag_queue = 0;

struct msg {
    long type;
    char msg[MAX_BUFFER];
} msg_snd, msg_rcv;

int str_to_positive_int(char * str) {
    if (strspn(str, "0123456789") == strlen(str)) {
        return atoi(str);
    } else {
        return -1;
    }
}

void child_handler(int signu) {
    if (signu == SIGUSR1) {
        flag_file = 1;
    } else if (signu == SIGUSR2) {
        flag_queue = 1;
    }
}

int main(int argc, char * argv[]) {
    int n;
    // Check args
    if (argc != 3) {
        printf("Usage: %s <path> <n>.\n", argv[0]);
        exit(ERR_ARGS);
    } else {
        n = str_to_positive_int(argv[2]);
        if (n < 1 || n > CHILDREN_MAX) {
            printf("%sn must be a valid integer from 1 to %d.\n%s", RED, CHILDREN_MAX, DF);
            exit(ERR_ARGS);
        }
    }

    char info_path[PATH_MAX_LENGTH - 12];
    snprintf(info_path, PATH_MAX_LENGTH - 12, "%s/info/", argv[1]);
    // Create dir
    if (mkdir(info_path, 0755) == -1) {
        if(errno != EEXIST) {
            snprintf(error_buffer, MAX_BUFFER, "%smkdir failed %s%s", RED, argv[1], DF);
            perror(error_buffer);
            exit(ERR_PATH);
        }
    }

    // Create mq
    key_t key = ftok(argv[1], FTOK_ID);
    if (key == -1) {
        snprintf(error_buffer, MAX_BUFFER, "%screate key with ftok failed %s", RED, DF);
        perror(error_buffer);
        exit(ERR_MQ);
    }
    // Remove queue if exists
    msgctl(key, IPC_RMID, NULL);
    int queueId = msgget(key, IPC_CREAT | O_WRONLY | 0755);
    if (queueId == -1) {
        snprintf(error_buffer, MAX_BUFFER, "%screate message queue failed %s", RED, DF);
        perror(error_buffer);
        exit(ERR_MQ);
    }

    // Create key.txt
    snprintf(path, PATH_MAX_LENGTH, "%s/key.txt", info_path);    
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | 0755);
    if (fd == -1) {
        snprintf(error_buffer, MAX_BUFFER, "%sopen failed %s%s", RED, path, DF);
        perror(error_buffer);
        exit(ERR_FQ);
    }
    // Write pid to key.txt and to queue
    pid_t master_pid = getpid();
    snprintf(buffer, MAX_BUFFER, "%d\n", master_pid);
    if (write(fd, buffer, strlen(buffer)) == -1) {
        perror("Write to key.txt failed");
        exit(ERR_FQ);
    }
    msg_snd.type = 1;
    strncpy(msg_snd.msg, buffer, strlen(buffer)-1);
    if(msgsnd(queueId, &msg_snd, strlen(msg_snd.msg), 0) == -1) {
        perror("Write to queue failed");
        exit(ERR_FQ);
    }
    pid_t pid;
    // Generate n process
    for (int i = 0; i <n; i++) {
        pid = fork();
        if (pid == 0) {
            // printf("[%d] I'm a new child.\n", getpid());
            break;
        } else {
            snprintf(path, MAX_BUFFER, "%s/%d.txt", info_path, pid);
            int fd_pid = open(path, O_CREAT | 0755);
            if (fd_pid == -1) {
                snprintf(error_buffer, MAX_BUFFER, "%sopen failed %s%s", RED, path, DF);
                perror(error_buffer);
                exit(ERR_FQ);
            }
            printf("%d ", pid);
            close(fd_pid);
        }
    }
    // Parent
    if (pid != 0) {
        printf("\n");
        // while(wait(NULL) != -1);
    }
    // Children
    if (pid == 0) {
        snprintf(path, PATH_MAX_LENGTH, "%s/%d.txt", info_path, getpid());
        int child_fd = open(path, O_WRONLY);
        if (child_fd == -1) {
            perror("open child fd");
            exit(ERR_FQ);
        }

        struct sigaction sa;
        sa.sa_handler = child_handler;
        sa.sa_flags = 0;
        sigaction(SIGUSR1, &sa, NULL);
        sigaction(SIGUSR2, &sa, NULL);
        while(1) { 
            if (flag_file) {
                printf("Write on file SIGUSR1.\n");
                snprintf(buffer, strlen("SIGUSR1\n") + 1, "SIGUSR1\n");
                if (write(child_fd, buffer, strlen(buffer)) == -1) {
                    perror("write to file failed");
                }
                flag_file = 0;
            }
            if (flag_queue) {
                msg_snd.type = 1;
                printf("Write on queue my pid %d\n", getpid());
                snprintf(msg_snd.msg, PIDLEN, "%d", getpid());
                msgsnd(queueId, &msg_snd, strlen(msg_snd.msg), 0);
                flag_queue = 0;
            }
            pause();
        }
    }

    return 0;
}