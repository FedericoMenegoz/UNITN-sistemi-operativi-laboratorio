#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define ERR_ARGS 1
#define ERR_FILE 2

#define PARAM_LEN 255


// Defining flags
int wait_flag = 1, queue_flag = 0, fifo_flag = 0, quit = 0;

char command[3][PARAM_LEN];

void handler(int signu) {
    if (signu == SIGUSR1) {
        wait_flag = 0;
    }
}
void *kill_routine( void *args) {
    int * data = ( int *) args;
    if (kill(data[1], data[0]) == -1) {
        perror("kill");
    }
    return NULL;
}

void *queue_routine(void *args) {
    char * path = ( char *) args;

    int key = ftok(path, 1);
    if (key == -1) {
        perror("ftok");
    }
    int queueId = msgget(key, IPC_CREAT | 0777);
    if (queueId == -1) {
        perror("msgget");
    }
    struct {
        long type;
        char mtext[PARAM_LEN];
    } msg_snd;

    while(1) {
        if(queue_flag) {
            msg_snd.type = atoi(command[1]);
            strcpy(msg_snd.mtext, command[2]);

            if (msgsnd(queueId, &msg_snd, sizeof(msg_snd.mtext), IPC_NOWAIT) == -1) {
                perror("msgsnd");
            }
            queue_flag = 0;
        }
        if (quit) {
            break;
        }
    }
    return NULL;
}

void *fifo_routine(__attribute__((unused)) void *args) {
    if (mkfifo(command[1], O_CREAT | 0777) == -1 && errno != EEXIST) {
        perror("mkfifo");
    }

    int fd = open(command[1], O_WRONLY);
    if (fd == -1) {
        perror("open");
    }
    if (write(fd, command[2], strlen(command[2])) == -1) {
        perror("write");
    }
    return NULL;
}

int main(int argc, char * argv[]) {
    // check args
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path/to/file.txt>.\n", argv[0]);
        return ERR_ARGS;
    }

    FILE * file = fopen(argv[1], "r");
    if (!file) {
        perror("fopen");
        return ERR_FILE;
    }

    // Debug
    printf("[%d] Running...\n", getpid());
    // Defining pthread variables
    pthread_t kill_th, queue_th, fifo_th;
    // pthread_mutex_t queue_mutex, fifo_mutex;

    if (pthread_create(&queue_th, NULL, queue_routine, argv[1]) == -1) {
        perror("pthread_create queue");
        return 3;
    }
    // Setting handler for SIGUSR1
    signal(SIGUSR1, handler);

    while(fscanf(file, "%s %s %s\n", command[0], command[1], command[2]) != EOF) {
        wait_flag = 1;
        // Kill 
        if (strcmp(command[0], "kill") == 0) {
            int args[] = {atoi(command[1]), atoi(command[2])} ;
            if (pthread_create(&kill_th, NULL, kill_routine, args) == -1) {
                perror("pthread_create for kill");
            }
        } 
        // Queue
        else if (strcmp(command[0], "queue") == 0) {
            queue_flag = 1;
        } 
        // Fifo
        else if (strcmp(command[0], "fifo") == 0) {
            if (pthread_create(&fifo_th, NULL, fifo_routine, NULL) == -1) {
                perror("pthread_create fifo");
            }
        }
        // Error
        else {
            fprintf(stderr, "Command '%s' not recognised.\n", command[0]);
        }
        // Wait for SIGUSR1
        while(wait_flag) {
            pause();
        }
        sleep(1);
    }
    quit = 1;
    pthread_join(kill_th, NULL);
    pthread_join(queue_th, NULL);
    pthread_join(fifo_th, NULL);
    return 0;
}