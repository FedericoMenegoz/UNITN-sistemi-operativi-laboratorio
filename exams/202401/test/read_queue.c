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

#define MQ_ID_STRING "/tmp/file"
#define MQ_ID_NUMBER 5

struct msg_q {
    long type;
    char text[100];
} msg_rcv;

int main(void) {
    

    int key = ftok(MQ_ID_STRING, MQ_ID_NUMBER);

    int queueId = msgget(key, O_RDONLY);
    
    if (queueId == -1) {
        perror("msgget");
        exit(1);
    }


    while (msgrcv(queueId, &msg_rcv, sizeof(msg_rcv.text), 0, IPC_NOWAIT) != -1) {
        printf("Message: %s\n", msg_rcv.text);
    }

    return 0;   
}