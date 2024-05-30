#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>


#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define DF "\033[0m"
#define BOLD "\033[1m"
#define UNBOLD "\033[m"

#define MAX_BUFFER 256
#define MAX_WORD 256

const char * SIG_REC = "./test/signal_receiver.out";
const char * PROGRAM = "./sol/out/a.out";
const char * FIFO = "./test/esame202306_fifo";
const char * CMD_FILE = "./test/esame202306_cmd";

const char *WORDS[] = {
    "I",
    "won't",
    "pass",
    "operating",
    "system",
    "lab",
    "exam!"
};

void success(char *msg) {
    printf("%s%s%s SUCCESS ! %s%s\n",BOLD, msg, GREEN, UNBOLD, DF);
}

void fail(char *msg) {
    printf("%s%s%s FAILURE !%s%s\n",BOLD, msg, RED, UNBOLD, DF);
}

int queue_fail = 0, fifo_fail = 0, signal_receiver_pid, program_pid, sig1_flag, sig2_flag;


struct msg {
    long category;
    char text[MAX_WORD];
} msg_rcv;

void handler(int signu, siginfo_t * info, __attribute__((unused)) void* ucontext) {
    if (signu == SIGUSR1) {
        if (info->si_pid == signal_receiver_pid) {
            sig1_flag = 1;
        }
    }
    if (signu == SIGUSR2) {
        if (info->si_pid == signal_receiver_pid) {
            sig2_flag = 1;
        }
    }
}

void *queue_reader_routine(__attribute__((unused)) void *args) {
    char msg[MAX_BUFFER];
    int key = ftok(CMD_FILE, 1);
    if (key == -1) {
        perror("ftok");
        queue_fail = 1;
    }
    int queueId = msgget(key, IPC_CREAT | O_RDONLY | 0777);

    if (queueId == -1) {
        perror("msgget");
        queue_fail = 1;
    }
    int messages = sizeof(WORDS) / sizeof(char *);
    for (int i = 0; i < messages && !queue_fail; i++) {
        int bytes = msgrcv(queueId, &msg_rcv, sizeof(msg_rcv.text), 0, 0);

        if (bytes == -1) {
            perror("Reading the queue");
            queue_fail = 1;
        }

        if (strcmp(msg_rcv.text, WORDS[i]) != 0) {
            snprintf(msg, MAX_BUFFER * 2, "Queue msg %s != %s", msg_rcv.text, WORDS[i]);
            fail(msg);
            queue_fail = 1;
        } if (msg_rcv.category != i + 1) {
            snprintf(msg, MAX_BUFFER, "Queue cat %ld != %d", msg_rcv.category, i + 1);
            fail(msg);
            queue_fail = 1;
        }
        else {
            kill(program_pid, SIGUSR1);
        }
    }
    if (msgctl(queueId, IPC_RMID, NULL) == -1) {
        perror("Deleting the queue");
    }
    return NULL;
}

void *fifo_reader_routine(__attribute__((unused)) void *args) {
    if (mkfifo(FIFO, O_CREAT | O_EXCL | 0666) == -1) {
        perror("mkfifo");
        fifo_fail = 1;
    }

    int fd = open(FIFO, O_RDONLY);

    if (fd == -1) {
        perror("fifo");
        fifo_fail = 1;
    }
    char fifo_buffer[MAX_BUFFER];
    int bytes;
    int messages = sizeof(WORDS) / sizeof(char *);
    for (int i = 0; i < messages && !fifo_fail; i ++) {
        size_t word_len = strlen(WORDS[i]) / sizeof(char);
        bytes = read(fd, fifo_buffer, word_len);
        if (bytes == -1) {
            perror("Reading fifo");
            fifo_fail = 1;
        }
        fifo_buffer[bytes] = 0;

        if (strcmp(fifo_buffer, WORDS[i]) != 0) {
            snprintf(fifo_buffer, MAX_BUFFER * 2, "Fifo msg %s != %s", msg_rcv.text, WORDS[i]);
            fail(fifo_buffer);
            fifo_fail = 1;
        } else {
            kill(program_pid, SIGUSR1);
        }
    }
    if (remove(FIFO) == -1) {
        perror("removing fifo");
    }
    return NULL;
}

int main(void) {
    // Remove all previous test files if any
    remove(CMD_FILE);
    remove(FIFO);

    char parent_pid[8];
    snprintf(parent_pid, 8, "%d", getpid());
    printf("%s[%d] TEST PROGRAM%s\n",BOLD, getpid(), UNBOLD);
    // Create the signal receiver process
    signal_receiver_pid = fork();

    if (signal_receiver_pid == -1) {
        perror("fork");
        exit(1);
    } else if (signal_receiver_pid == 0) {
        if (execl(SIG_REC, SIG_REC, parent_pid, NULL) == -1) {
            fprintf(stderr, "running %s ", SIG_REC);
            perror("");
            exit(2);
        }

    }
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    int file = open(CMD_FILE, O_CREAT | O_EXCL | O_WRONLY, 0666);

    if (file == -1) {
        perror("opening file");
        exit (1);
    }

    // write kill sigusr1 and sigusr2 to the cmd file
    char buffer[MAX_BUFFER];
    snprintf(buffer, MAX_BUFFER, "kill %d %d\n", SIGUSR1, signal_receiver_pid);
    if (write(file, buffer, strlen(buffer)) == -1) {
        perror("writing to buffer");
        exit(1);
    }
    snprintf(buffer, MAX_BUFFER, "kill %d %d\n", SIGUSR2, signal_receiver_pid);
    if (write(file, buffer, strlen(buffer)) == -1) {
        perror("writing to buffer");
        exit(1);
    }
    int messages = sizeof(WORDS) / sizeof(char *);
    for (int i = 0; i < messages; i++) {
        snprintf(buffer, MAX_BUFFER, "queue %d %s\nfifo %s %s\n", i + 1, WORDS[i], FIFO, WORDS[i]);
        if (write(file, buffer, strlen(buffer)) == -1) {
           perror("writing to buffer");
            exit(1);
        }      
    }
    close(file);

    // Running the solution program
    program_pid = fork();
    if(program_pid == -1) {
        perror("fork");
        exit(1);
    } else if (program_pid == 0) {
        if (execl(PROGRAM, PROGRAM, CMD_FILE, NULL) == -1) {
            perror("Running solution program");
            exit(2);
        }
    }


    pthread_t queue_reader, fifo_reader;
    
    if (pthread_create(&queue_reader, NULL, queue_reader_routine, NULL) != 0) {
        perror("Pthread create");
        exit(1);
    }

    if (pthread_create(&fifo_reader, NULL, fifo_reader_routine, NULL) != 0) {
        perror("Pthread create");
        exit(1);
    }

    // wait for sigusr1 and sigusr2
    sleep(2);
    if (sig1_flag) {
        success("SIGUSR1");
    } else {
        fail("SIGUSR1");
    }
    sleep(2);
    if (sig2_flag) {
        success("SIGUSR2");
    } else {
        fail("SIGUSR2");
    }
    printf("%s[%d] Reading queue and fifo ... this may a take few seconds ...%s\n", BOLD, getpid(), UNBOLD);
    pthread_join(fifo_reader, NULL);
    pthread_join(queue_reader, NULL);

    if (queue_fail) {
        fail("Queue messages match.");
    } else {
        success("Queue messages match.");
    }

    if (fifo_fail) {
        fail("Fifo messages match.");
    } else {
        success("Fifo messages match.");
    }

    kill(signal_receiver_pid, SIGTERM);
    remove(FIFO);
    return 0;
}
