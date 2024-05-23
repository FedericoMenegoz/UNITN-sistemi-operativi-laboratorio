#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define ERR_ARGS 1
#define ERR_QUEUE 2
#define ARG_LEN 32
#define MAX_BUFFER 64
#define FTOK_ID 1
#define RED "\033[0;31m"
#define DF "\033[0m"

struct msg_q {
	long type;
	char text[ARG_LEN];
} msg_snd, msg_rcv;

/**
 * Opens a message queue with the given key. If the queue does not exist it will create it.
 *
 * @param key The key used to identify the message queue.
 * @return The ID of the opened message queue. -1 if an error occured.
 */
int open_queue(key_t key);

/**
 * Exits the program and sends a signal to the specified process.
 *
 * @param ex_code The exit code of the program. If 0 it will send SIGUSR1, SIGUSR2 otherwise
 */
void exit_and_signal(int ex_code);

/**
 * Converts a string to a positive integer.
 *
 * @param str The string to convert.
 * @return The converted integer value, or -1 if the conversion failed.
 */
int str_to_positive_int(char *str);

/**
 * Prints an error message and exits the program with the specified error code.
 *
 * @param msg The error message to print.
 * @param err The error code.
 */
void perr(char *msg, int err);

/**
 * Deletes the specified message queue.
 *
 * @param queueId The ID of the message queue to delete.
 */
void delete_queue(int queueId);

pid_t ext_pid = 0;
char buffer[MAX_BUFFER];

int main(int argc, char *argv[]) {
	// Check if arguments are correct.
	if (argc < 4 || argc > 5) {
		printf("%sUsage: %s <name> <action> [<value>] <pid>%s\n", RED, argv[0], DF);
		exit(ERR_ARGS);
	} else {
		ext_pid = str_to_positive_int(argv[argc - 1]);
		if (ext_pid < 1) {
			printf("%sPid argument need to be a valid integer.%s\n", RED, DF);
			exit(ERR_ARGS);
		}
	}
	// Retrieve queue key.
	key_t key = ftok(argv[1], FTOK_ID);
	if (key == -1) {
		perr("Error generating the queue key. ftok", ERR_QUEUE);
	}

	int queueId = open_queue(key);
	// NEW GET EMP DEL
	if (argc == 4) {
		// new
		if (strcmp(argv[2], "new") == 0) {
			queueId = open_queue(key);
			printf("%d\n", queueId);
		}
		// get
		else if (strcmp(argv[2], "get") == 0) {
			// get message if any
			if (msgrcv(queueId, &msg_rcv, sizeof(msg_rcv.text), 0, IPC_NOWAIT) ==
					-1) {
				if (errno != ENOMSG) {
					perr("Error getting the message from queue. msgrcv", ERR_QUEUE);
				}
			} else {
				printf("%s\n", msg_rcv.text);
			}
		}
		// del
		else if (strcmp(argv[2], "del") == 0) {
			delete_queue(queueId);
		}
		// emp
		else if (strcmp(argv[2], "emp") == 0) {

			while (msgrcv(queueId, &msg_rcv, sizeof(msg_rcv.text), 0, IPC_NOWAIT) !=
						 -1) {
				printf("%s\n", msg_rcv.text);
			}
		} else {
			printf(
					"\n%sWrong args. Specify a value only when action is 'put' or 'move'.\n%s\n", RED, DF);
			exit_and_signal(ERR_ARGS);
		}
	}
	// PUT MOV
	else if (argc == 5) {
		// put
		if (strcmp(argv[2], "put") == 0) {
			// prepare msg
			strcpy(msg_snd.text, argv[3]);
			msg_snd.type = 1;
			// add message to queue
			if (msgsnd(queueId, &msg_snd, strlen(msg_snd.text), 0) == -1) {
				perr("\nError putting message into queue. msgsnd", ERR_QUEUE);
			}
		}
		// mov
		else if (strcmp(argv[2], "mov") == 0) {
			// prepare new queue
			int key2 = ftok(argv[3], FTOK_ID);
			if (key2 == -1) {
				perr("\nError creating destination queue. ftok", ERR_QUEUE);
			}
			int queueId2 = open_queue(key2);
			// move messages
			while (msgrcv(queueId, &msg_rcv, sizeof(msg_rcv.text), 0, IPC_NOWAIT) !=
						 -1) {

				printf("%s\n", msg_rcv.text);
				if (msgsnd(queueId2, &msg_rcv, strlen(msg_rcv.text), 0) == -1) {
					perr("\nError transferring the message to the destination queue. msgsnd",
							 ERR_QUEUE);
				}
			}
			if (errno != ENOMSG) {
				perr("\nError reading the source queue. msgrcv", ERR_QUEUE);
			}
			delete_queue(queueId);

		} else {
			printf("%s\nWrong args. Specify a value only when action is 'put' or "
						 "'move'.\n\n%s",
						 RED, DF);
			exit(ERR_ARGS);
		}
	}
	// Success
	exit_and_signal(0);
}

void delete_queue(int queueId) {
	if (msgctl(queueId, IPC_RMID, NULL) == -1) {
		perr("Error deleting the queue. msgctl", ERR_QUEUE);
	}
}

int open_queue(key_t key) {
	int queueId = msgget(key, IPC_CREAT | IPC_EXCL | O_RDWR | 0666);
	if (queueId == -1) {
		if (errno == EEXIST) {
			queueId = msgget(key, O_RDWR);
			if (queueId == -1) {
				perr("\nError opening the queue. msgget", ERR_QUEUE);
			}
		}
	}
	return queueId;
}

void exit_and_signal(int ex_code) {
	if (ex_code) {
		kill(ext_pid, SIGUSR2);
		exit(ex_code);
	} else {
		kill(ext_pid, SIGUSR1);
		exit(0);
	}
}

int str_to_positive_int(char *str) {
	int ret =  atoi(str);
	if (strspn(str, "0123456789") == strlen(str) && ret > 0)
		return ret;
	else
		return -1;
}

void perr(char *msg, int err) {
	snprintf(buffer, MAX_BUFFER, "%s%s%s", RED, msg, DF);
	perror(buffer);
	printf("\n");
	exit_and_signal(errno);
}