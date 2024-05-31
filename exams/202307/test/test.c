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
#include <sys/wait.h>
#include <unistd.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define DF "\033[0m"
#define BOLD "\033[1m"
#define UNBOLD "\033[m"

#define MAX_LINE 256
#define MAX_BUFFER 512
#define MAX_WORD 256

#define TASK1 3
#define TASK2 4
#define TASK3 5
#define TASK4 5
#define TASK5 6
#define TASK6 3
#define TASK7 5

#define TEST_FILE "./test/file_test.txt"
#define PROGRAM "./sol/PostOffice"
#define SIG_REC "./test/signal_receiver.out"
#define LOG_ERR "./test/log_err.txt"
#define NOT_EXIST "./test/dir/dir/not_exist"

const char *WORDS[] = {"Passing",   "operating", "system", "is",  "like",
                       "getting",   "a",         "Roman",  "to",  "cook",
                       "carbonara", "with",      "cream",  "and", "ham!"};

void success(char *msg) {
    printf("%s%s%s SUCCESS ! %s%s\n", BOLD, msg, GREEN, UNBOLD, DF);
}
void fail(char *msg) {
    printf("%s%s%s FAILURE !%s%s\n", BOLD, msg, RED, UNBOLD, DF);
}
void evaluate(char *msg, int passed) {
    if (passed) {
        success(msg);
    } else {
        fail(msg);
    }
}

int task1_checking_args(void);
int task2_create_workers(void);
int task3_handling_sigusr1_2(void);

int main(void) {
    int score = 0;
    remove(LOG_ERR);
    int tasks[7];

    tasks[0] = task1_checking_args();
    evaluate("TASK 1 (arguments checking)", tasks[0]);

    tasks[1] = task2_create_workers();
    evaluate("TASK 2 (creating workers)", tasks[1]);

    tasks[2] = task3_handling_sigusr1_2();
    evaluate("TASK 3 (handling SIGUSR1-2)", tasks[2]);

    remove(LOG_ERR);
    return 0;
}

int task1_checking_args(void) {
    int pid_fork, first_fork, status, bytes, offset = 0;

    printf("[%d] Test program\n", getpid());
    // Save stderr
    int stderr_fd = dup(STDERR_FILENO);
    int logerr_fd = open(LOG_ERR, O_CREAT | O_RDWR, 0777);
    if (dup2(logerr_fd, STDERR_FILENO) == -1) {
        perror("dup2");
        return -1;
    }
    // wrong number of args
    first_fork = fork();
    if (first_fork == 0) {
        if (execl(PROGRAM, PROGRAM, NULL) == -1) {
            perror("execl");
        }
        exit(-1);
    }
    wait(&status);
    char buffer[MAX_BUFFER];
    lseek(logerr_fd, 0, SEEK_SET);
    bytes = read(logerr_fd, buffer, 256);
    buffer[bytes - 1] = 0;
    offset += bytes;
    if (WEXITSTATUS(status) == 0 || bytes == 0) {
        return 0;
    }

    // wrong n
    pid_fork = fork();
    if (pid_fork == 0) {
        if (execl(PROGRAM, PROGRAM, "12", "bubusettete", "some pid", NULL) ==
            -1) {
            perror("execl");
        }
        exit(-1);
    }
    wait(&status);
    lseek(logerr_fd, offset, SEEK_SET);
    bytes = read(logerr_fd, buffer, 256);
    buffer[bytes - 1] = 0;
    offset += bytes;
    if (WEXITSTATUS(status) == 0 || bytes == 0) {
        return 0;
    }

    // file exists?
    pid_fork = fork();
    if (pid_fork == 0) {
        if (execl(PROGRAM, PROGRAM, "10", NOT_EXIST, "some pid", NULL) == -1) {
            perror("execl");
        }
        exit(-1);
    }
    wait(&status);
    lseek(logerr_fd, offset, SEEK_SET);
    bytes = read(logerr_fd, buffer, 256);
    buffer[bytes - 1] = 0;
    offset += bytes;
    if (WEXITSTATUS(status) == 0 || bytes == 0) {
        return 0;
    }

    // Is pid valid first_fork pid is dead already.
    pid_fork = fork();
    snprintf(buffer, MAX_BUFFER, "%d", first_fork);
    if (pid_fork == 0) {
        if (execl(PROGRAM, PROGRAM, "10", TEST_FILE, buffer, NULL) == -1) {
            perror("execl");
        }
        exit(-1);
    }
    wait(&status);
    lseek(logerr_fd, offset, SEEK_SET);
    bytes = read(logerr_fd, buffer, 256);
    buffer[bytes - 1] = 0;
    offset += bytes;
    if (WEXITSTATUS(status) == 0 || bytes == 0) {
        return 0;
    }

    // Let's call it with everything right
    int signal_receiver_pid = fork();
    if (signal_receiver_pid == 0) {
        if (execl(SIG_REC, SIG_REC, "1", NULL) == -1) {
            perror("execl");
        }
    }
    snprintf(buffer, MAX_BUFFER, "%d", signal_receiver_pid);

    pid_fork = fork();

    if (pid_fork == 0) {
        if (execl(PROGRAM, PROGRAM, "1", TEST_FILE, buffer, NULL) == -1) {
            perror("execl");
        }
        exit(-1);
    }
    sleep(1);
    kill(signal_receiver_pid, SIGINT);
    kill(-pid_fork, SIGTERM);
    wait(&status);
    lseek(logerr_fd, offset, SEEK_SET);
    bytes = read(logerr_fd, buffer, 256);
    buffer[bytes - 1] = 0;
    offset += bytes;
    if (WEXITSTATUS(status) != 0) {
        return 0;
    }

    close(logerr_fd);
    dup2(stderr_fd, STDERR_FILENO);
    // All good!
    return TASK1;
}

int task2_create_workers(void) {
    int signal_receiver_pid, program_pid, status;
    char *N = "10";
    char buffer[MAX_BUFFER];
    signal_receiver_pid = fork();

    if (signal_receiver_pid == 0) {
        if (execl(SIG_REC, SIG_REC, N, NULL) == -1) {
            perror("execl");
        }
    }

    snprintf(buffer, MAX_BUFFER, "%d", signal_receiver_pid);
    program_pid = fork();
    if (program_pid == 0) {
        if (execl(PROGRAM, PROGRAM, N, TEST_FILE, buffer, NULL) == -1) {
            perror("execl");
        }
    }

    sleep(1);
    kill(-program_pid, SIGTERM);
    kill(signal_receiver_pid, SIGINT);
    usleep(500000);
    wait(&status);
    if (WEXITSTATUS(status) == 0) {
        return TASK2;
    } else {
        return 0;
    }
}

// flags
int task3_sig1 = 0, task3_sig2 = 0, worker_counter = 0;
#define N_T3 3
pid_t workers_t3[N_T3];

void handler_task3(int signu, siginfo_t *info,
                   __attribute__((unused)) void *ucontext) {
    if (signu == SIGUSR1) {
        task3_sig1 = 1;
    }
    if (signu == SIGUSR2) {
        task3_sig2 = 1;
    }
    if (signu == SIGTERM) {
        workers_t3[worker_counter++] = info->si_pid;
    }
}

int task3_handling_sigusr1_2(void) {
    int program_pid;
    char buffer[MAX_BUFFER];
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler_task3;
    sigemptyset(&act.sa_mask);

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);

    snprintf(buffer, MAX_BUFFER, "%d", getpid());
    program_pid = fork();
    if (program_pid == 0) {
        if (execl(PROGRAM, PROGRAM, "3", TEST_FILE, buffer, NULL) == -1) {
            perror("execl");
            exit(1);
        }
    }
    sleep(2);
    for (int i = 0; i < N_T3; i++) {
        kill(workers_t3[i], SIGUSR1);
        usleep(500000);
        if (!task3_sig1) {
            return 0;
        }
        task3_sig1 = 0;
        kill(workers_t3[i], SIGUSR2);
        usleep(500000);
        if (!task3_sig2) {
            return 0;
        }
        kill(workers_t3[i], SIGUSR1);
        usleep(500000);
        if (task3_sig1) {
            return 0;
        }
        printf("%s[%d] Pid %d responded correctly%s\n", BOLD, getpid(),
               workers_t3[i], UNBOLD);
    }
    kill(-program_pid, SIGTERM);
    while (wait(NULL) > 0);
    return TASK3;
}