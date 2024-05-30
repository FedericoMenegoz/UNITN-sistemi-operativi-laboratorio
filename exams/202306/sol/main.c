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

#define ERR_ARG 1
#define ERR_FILE 2

#define MAX_BUFFER 256

int kill_flag = 0, fifo_flag = 0, queue_flag = 0, wait_flag;
pthread_mutex_t kill_mtx, fifo_mtx, queue_mtx;
pthread_cond_t kill_cond, fifo_cond, queue_cond;
pid_t pid_to_kill;
int signu;
char word[MAX_BUFFER];
char fifo_name[MAX_BUFFER];

void handler(int signu) {
    if (signu == SIGUSR1) {
        wait_flag = 0;
    }
}

struct msg{
    long category;
    char text[MAX_BUFFER];
} msg_snd;

void *kill_routine(__attribute__((unused)) void *args){
    while(1) {
        pthread_mutex_lock(&kill_mtx);
        while (!kill_flag) {
            pthread_cond_wait(&kill_cond, &kill_mtx);
        }
        kill(pid_to_kill, signu);
        kill_flag = 0;
        pthread_mutex_unlock(&kill_mtx);
    }
    return NULL;
}

void *fifo_routine(__attribute__((unused)) void *args){
    while(1) {
        pthread_mutex_lock(&fifo_mtx);
        while(!fifo_flag) {
            pthread_cond_wait(&fifo_cond, &fifo_mtx);
        }
        int fd = open(fifo_name, O_WRONLY | O_CREAT, 0777);
        if (fd == -1) {
            perror("opening fifo");
        }
        if (write(fd, word, strlen(word)) == -1) {
            perror("write");
        }
        fifo_flag = 0;
        pthread_mutex_unlock(&fifo_mtx);
    }
    return NULL;
}
void *queue_routine(void *args){
    int qID = *(int *) args;
    while(1) {
        pthread_mutex_lock(&queue_mtx);
        while(!queue_flag) {
            pthread_cond_wait(&queue_cond, &queue_mtx);
        }
        if (msgsnd(qID, &msg_snd, sizeof(msg_snd.text), 0) == -1) {
            perror("Sending message to queue");
        }
        queue_flag = 0;
        pthread_mutex_unlock(&queue_mtx);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path/to/file.txt>\n", argv[0]);
        exit(ERR_ARG);
    }
    FILE * cmd_file = fopen(argv[1], "r");

    if (!cmd_file) {
        perror("Error opening the file");
        exit(ERR_FILE);
    }
    int key = ftok(argv[1], 1);
    if (key == -1) {
        perror("key");
    }
    int queueId = msgget(key, IPC_CREAT | O_WRONLY | 0777);
    if (queueId == -1) {
        fprintf(stderr, "Message queue could not be created");
        perror("");
        exit(ERR_FILE);
    }
    signal(SIGUSR1, handler);

    pthread_t kill_th, queue_th, fifo_th;
   
    pthread_mutex_init(&kill_mtx, NULL);
    pthread_mutex_init(&queue_mtx, NULL);
    pthread_mutex_init(&fifo_mtx, NULL);
    pthread_cond_init(&kill_cond, NULL);
    pthread_cond_init(&queue_cond, NULL);
    pthread_cond_init(&fifo_cond, NULL);
    
    pthread_create(&kill_th, NULL, kill_routine, NULL);
    pthread_create(&queue_th, NULL, queue_routine, &queueId);
    pthread_create(&fifo_th, NULL, fifo_routine, NULL);


    char cmd[MAX_BUFFER];
    char arg1[MAX_BUFFER];
    char arg2[MAX_BUFFER];

    while(fscanf(cmd_file, "%s %s %s", cmd, arg1, arg2) != -1) {
        wait_flag = 1;
        if (strcmp(cmd, "kill") == 0) {
            pthread_mutex_lock(&kill_mtx);
            kill_flag = 1;
            pid_to_kill = atoi(arg2);
            signu = atoi(arg1);
            pthread_mutex_unlock(&kill_mtx);
            pthread_cond_signal(&kill_cond);
        } else if (strcmp(cmd, "queue") == 0) {
            pthread_mutex_lock(&queue_mtx);
            queue_flag = 1;
            msg_snd.category = atoi(arg1);
            strcpy(msg_snd.text, arg2);
            pthread_mutex_unlock(&queue_mtx);
            pthread_cond_signal(&queue_cond);            
        } else if (strcmp(cmd, "fifo") == 0) {
            pthread_mutex_lock(&fifo_mtx);
            fifo_flag = 1;
            strcpy(fifo_name, arg1);
            strcpy(word, arg2);
            pthread_mutex_unlock(&fifo_mtx);
            pthread_cond_signal(&fifo_cond);
        } else {
            fprintf(stderr, "Wrong command %s\n", cmd);
        }
        while(wait_flag);
        sleep(1);
    }
    pthread_join(kill_th, NULL);
    pthread_join(queue_th, NULL);
    pthread_join(fifo_th, NULL);
    pthread_cond_destroy(&kill_cond);
    pthread_cond_destroy(&queue_cond);
    pthread_cond_destroy(&fifo_cond);
    pthread_mutex_destroy(&kill_mtx);
    pthread_mutex_destroy(&queue_mtx);
    pthread_mutex_destroy(&fifo_mtx);

    return 0;
}