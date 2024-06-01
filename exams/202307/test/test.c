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


#define MAX_BUFFER 256

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

#define MESSAGES 15
const char *WORDS[] = {"Passing",   "operating", "systems", "is",  "like",
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

int task1_checking_args(void);
int task2_create_workers(void);
int task3_handling_sigusr1_2(void);
void task4_5_6_7_queue_transfer(int task[]);

int main(void) {
    int score = 0;
    remove(LOG_ERR);
    int tasks[7];

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler_task3;
    sigemptyset(&act.sa_mask);

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);

    tasks[0] = task1_checking_args();
    sleep(1);
    evaluate("TASK 1 (arguments checking)", tasks[0]);
    
    tasks[1] = task2_create_workers();
    sleep(1);
    evaluate("TASK 2 (creating workers)", tasks[1]);

    tasks[2] = task3_handling_sigusr1_2();
    sleep(1);
    evaluate("TASK 3 (handling SIGUSR1-2)", tasks[2]);

    task4_5_6_7_queue_transfer(tasks);
    sleep(1);
    evaluate("TASK 4 (message queue basic)", tasks[3]);
    sleep(1);
    evaluate("TASK 5 (message queue only workers)", tasks[4]);
    sleep(1);
    evaluate("TASK 6 (correct terminating of PostOffice and Workers)", tasks[5]);
    sleep(1);
    evaluate("TASK 7 (equal distribution of messages between workers)", tasks[6]);
    remove(LOG_ERR);

    for (int i = 0; i < 7; i++) {
        score += tasks[i];
    }
    printf("%s[%d] Total score is %d out of 31.%s\n", BOLD, getpid(), score, UNBOLD);
       
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


int task3_handling_sigusr1_2(void) {
    int program_pid;
    char buffer[MAX_BUFFER];

    snprintf(buffer, MAX_BUFFER, "%d", getpid());
    program_pid = fork();
    if (program_pid == 0) {
        if (execl(PROGRAM, PROGRAM, "3", TEST_FILE, buffer, NULL) == -1) {
            perror("execl");
            exit(1);
        }
    }
    usleep(500000);
    for (int i = 0; i < N_T3; i++) {
        kill(workers_t3[i], SIGUSR1);
        usleep(500000);
        usleep(500000);
        if (!task3_sig1) {
            return 0;
        }
        task3_sig1 = 0;
        for (int j = 0; j < N_T3; j++) {
            kill(workers_t3[i], SIGUSR2);
            usleep(500000);
            usleep(500000);
            if (!task3_sig2) {
                return 0;
            }
            task3_sig2 = 0;
        }
        kill(workers_t3[i], SIGUSR1);
        usleep(500000);
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

int is_worker(pid_t p) {
    for (int i = 0; i < N_T3; i++) {
        if (p == workers_t3[i]) return i;
    }
    return -1;
}

void task4_5_6_7_queue_transfer(int tasks[]) {
    int program_pid;
    worker_counter = 0;
    char buffer[MAX_BUFFER];
    tasks[3] = TASK4;
    tasks[4] = TASK5;
    tasks[5] = TASK6;
    tasks[6] = TASK7;
    snprintf(buffer, MAX_BUFFER, "%d", getpid());
    program_pid = fork();
    if (program_pid == 0) {
        if (execl(PROGRAM, PROGRAM, "3", TEST_FILE, buffer, NULL) == -1) {
            perror("execl");
            exit(1);
        }
    }
    while(worker_counter < N_T3);
    sleep(2);
    printf("%s[%d] Sendig SIGWINCH%s\n", BOLD, getpid(), UNBOLD);
    if (kill(program_pid, SIGWINCH) == -1) {
        perror("kill");
    }
    struct msg_buf {
        long type;
        char mtext[MAX_BUFFER];
    }msg_rcv;

    int key = ftok(TEST_FILE, program_pid);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    int queueId = msgget(key, O_RDONLY | 0777);
    if (queueId == -1) {
        perror("msgget");
        exit(1);
    }
    int res = 0;
    sleep(1);
    int w[] = {0, 0, 0};
    int index;
    for (int i = 0; i < MESSAGES; i++) {
        if ((res = msgrcv(queueId, &msg_rcv, sizeof(msg_rcv.mtext), 0, IPC_NOWAIT)) == -1 && errno != ENOMSG) {
            perror("msgrcv");
            exit(1);
        } else if (res == -1) {
            printf("%s[%d] %d message not received ...%s\n", BOLD, getpid(), i, UNBOLD);
            tasks[3] = tasks[6] = tasks[5] = tasks[4] = 0;
            return;
        }
        if (strcmp(msg_rcv.mtext, WORDS[i]) != 0) {
            printf("%s[%d] %s != %s %s\n", BOLD, getpid(), msg_rcv.mtext, WORDS[i], UNBOLD);
            tasks[3] = tasks[6] = tasks[5] = tasks[4] = 0;
            return;
        }
        if ((index = is_worker(msg_rcv.type)) >= 0) {
            w[index]++;
        } else {
            printf("%s[%d] Message \"%s\" from a non worker pid %ld %s\n", BOLD, getpid(), msg_rcv.mtext, msg_rcv.type, UNBOLD);
            tasks[6] = tasks[4] = 0;
        }
        sleep(1);
    }
    // 15 messages should be 5 messages each worker
    if (tasks[4]) {
        if (!(w[0] == w[1] && w[1] == w[2] && w[2] == 5)) {
            tasks[6] = 0;
        } 
    }

    sleep(1);
    // Check if children are dead.
    for (int i = 0; i < N_T3; i++) {
        if (kill(workers_t3[i], 0) == 0) {
            printf("%s[%d] Worker %d still reachable%s\n", BOLD, getpid(), workers_t3[i], UNBOLD);
            tasks[5] = 0;
            break;
        }
    }
    while(wait(NULL) > 0); 
    if (kill(program_pid, 0) == 0) {
        printf("%s[%d] PosdtOffice %d still reachable%s\n", BOLD, getpid(), program_pid, UNBOLD);
        tasks[5] = 0;
    }
    msgctl(queueId, IPC_RMID, NULL);

}

