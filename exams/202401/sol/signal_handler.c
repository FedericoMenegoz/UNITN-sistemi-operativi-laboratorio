#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_CHILDREN 10
#define MQ_ID_STRING "/tmp/file"
#define MQ_ID_NUMBER 5

pid_t children[MAX_CHILDREN];
int children_alive = 0, first_child = 0, last_child = 0;


// signal counters
int xcpu = 0, ttou = 0, ttin = 0;
// signal flags
int xcpu_f = 0, ttou_f = 0, ttin_f = 0, usr1_f = 0, fpe_f = 0, xfsz_f = 0, usr2_f = 0, quit_f = 0, prof_f = 0, read_fifo = 0;
// sender pids
int usr1_pid, fpe_pid;

void signal_counter(int signu, siginfo_t *info, __attribute__((unused)) void *ucontext) {
    if (signu == SIGXCPU) {
        xcpu_f = 1;
    }
    if (signu == SIGTTOU) {
        ttou_f = 1;
    }
    if (signu == SIGTTIN) {
        ttin_f = 1;
    }
    if (signu == SIGUSR1) {
        usr1_f = 1;
        usr1_pid = info->si_pid;
    }
    if (signu == SIGFPE) {
        fpe_f = 1;
        fpe_pid = info->si_pid;
    }
    if (signu == SIGUSR2) {
        usr2_f = 1;
    }
    if (signu == SIGXFSZ) {
        xfsz_f = 1;
    }
    if (signu == SIGQUIT) {
        quit_f = 1;
    }
    if (signu == SIGPROF) {
        prof_f = 1;
    }
}

void child_handler(int signu, siginfo_t *info, __attribute__ ((unused)) void *ucontext) {
    if (signu == SIGQUIT) {
        quit_f = 1;
    }
    if (signu == SIGINT) {
        read_fifo = 1;
    }
}

int main(void) {
    
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = signal_counter;
    if (sigaction(SIGXCPU, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGTTOU, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGTTIN, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGFPE, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGUSR2, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGXFSZ, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGQUIT, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGPROF, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
    // create queue
    if (creat(MQ_ID_STRING, 0777) == -1) {
        perror("creat");
        exit(1);
    }
    int key = ftok(MQ_ID_STRING, MQ_ID_NUMBER);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    int queueId = msgget(key, O_WRONLY | IPC_CREAT | 0777);
    if (queueId == -1) {
        perror("msgget");
        exit(1);
    }

    struct msg_buf{
        long type;
        char mtext[16];
    }msg_snd;
    char buffer[16];
    int child = -1, child_index;
    int fd[10][2];

    printf("[%d]\n", getpid());
    while(1) {
        // printf("cpu_f %d cpu_c %d\nttou_f %d ttou_c %d\nttin_f %d ttin_f %d\n", xcpu_f, xcpu, ttou_f, ttou, ttin_f, ttin);
        pause();
        if(xcpu_f) {
            printf("SIGXCPU_%d%c", ++xcpu, '\0');
            fflush(stdout);
            xcpu_f = 0;
        }
        if(ttou_f) {
            printf("SIGTTOU_%d%c", ++ttou, '\0');
            fflush(stdout);
            ttou_f = 0;
        }
        if(ttin_f) {
            printf("SIGTTIN_%d%c", ++ttin, '\0');
            fflush(stdout);
            ttin_f = 0;
        }
        if(usr1_f) {
            snprintf(msg_snd.mtext, 16, "SIGUSR1");
            msg_snd.type = usr1_pid;
            if (msgsnd(queueId, &msg_snd, sizeof(msg_snd.mtext), 0) == -1) {
                perror("msgsnd");
                exit(1);
            }
            usr1_f = 0;
        }
        if(fpe_f) {
            snprintf(msg_snd.mtext, 16, "SIGFPE");
            msg_snd.type = usr1_pid;
            if (msgsnd(queueId, &msg_snd, sizeof(msg_snd.mtext), 0) == -1) {
                perror("msgsnd");
                exit(1);
            }
            fpe_f = 0;
        }
        if(usr2_f) {
            if (children_alive < MAX_CHILDREN) {

                child = fork();

                if (child == -1) {
                    perror("fork");
                    exit(1);
                }

                if (pipe(fd[last_child]) == -1) {
                    perror("pipe");
                }

                if (child == 0) {
                    // prepare the child
                    child_index = last_child;
                    // close writing side of pipe
                    close(fd[last_child][1]);
                    printf("[%d] Child with index %d ...\n", getpid(), child_index);
                    act.sa_sigaction = child_handler;
                    sigaction(SIGQUIT, &act, NULL);
                    sigaction(SIGINT, &act, NULL);
                    break;
                }
                close(fd[last_child][0]);
                children_alive++;
                children[last_child] = child;
                last_child = (last_child + 1) % MAX_CHILDREN;

            } else {
                printf("[%d] too many children mate...\n", getpid());
            }
            usr2_f = 0;
        }
        if (xfsz_f) {
            if (children_alive > 0) {
                kill(children[first_child], SIGQUIT);
                first_child = (first_child + 1) % MAX_CHILDREN;
                children_alive--;
            } else {
                printf("[%d] Children all already dead.\n", getpid());
            }
            xfsz_f = 0;
        }
        if (quit_f) {
            kill(-getpid(), SIGQUIT);
            while(wait(NULL) > 0);
            break;
        }
        if (prof_f) {
            int before_last = (last_child - 2 + MAX_CHILDREN) % MAX_CHILDREN;
            int last = (last_child - 1 + MAX_CHILDREN) % MAX_CHILDREN;
            if (children_alive == 1) {
                printf("Writing on the pipe of %d the pid 0\n", children[last]); 
                printf("ONly one child\n");
                strcpy(buffer, "0");
                if (write(fd[last][1], buffer, strlen(buffer)) == -1) {
                    perror("write");
                } 
                kill(children[last], SIGINT);
            }
            else if (children_alive > 1) {
                printf("Writing on the pipe of %d the pid %d\n", children[last], children[before_last]); 
                printf("ONly %d child\n", children_alive);
                snprintf(buffer, 16, "%d", children[before_last]);
                printf("ONly %d child\n", children_alive);
                if(write(fd[last][1], buffer, strlen(buffer)) == -1) {
                    perror("write");
                }
                printf("ONly %d child\n", children_alive);
                // printf("Wrote %s on pipe\n", buffer);
                // kill(children[last], SIGINT);
            } else {
                printf("[%d] No children..\n", getpid());
            }
            prof_f = 0;
            printf("Going out\n");
        }
    }

    // Child process
    while(child == 0) {
        if (quit_f) {
            fprintf(stderr, "[%d] QUITTING%c", getpid(), '\0');
            break;
        }
        if (read_fifo) {
            printf("Received sigint\n");
            sleep(1);
            int bytes = read(fd[child_index][0], buffer, 16);
            if(bytes == -1) {
                perror("read");
                exit(1);
            }
            buffer[bytes] = '\0';
            fprintf(stderr, "%s bytes:%d", buffer, bytes);
            read_fifo = 0;
            printf("Written to stderr\n");
        }
    }
    return 0;
}
