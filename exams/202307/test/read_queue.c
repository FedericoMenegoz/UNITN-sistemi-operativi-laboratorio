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

struct msg_q {
    long type;
    char text[256];
} msg_rcv;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <path> <master pid>\n ", argv[0]);
    }

    int key = ftok(argv[1], atoi(argv[2]));

    int queueId = msgget(key, O_RDONLY);
    
    if (queueId == -1) {
        perror("msgget");
        exit(1);
    }

    int res;
    while ((res = msgrcv(queueId, &msg_rcv, sizeof(msg_rcv.text), 0, IPC_NOWAIT)) != -1) {
        printf("Message: %s\n", msg_rcv.text);
    }

    if (res == -1) {
        perror("res");
    }


    return 0;   
}