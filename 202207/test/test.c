#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../sol/lib.c"

#define SUCCESS 1
#define FAILURE 0

#define NOT_EXITED 2
#define NOT_STARTED 3
#define NOT_MATCHING 4
#define EXTRA_CHAR 5

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define DF "\033[0m"
#define BOLD "\033[1m"
#define UNBOLD "\033[m"

#define MSG_BUFFER 64

const char *PROGRAMS_PATH = "./sol/";
const char *WRITER_PATH = "./test/writer";
const char *FIFO_PATH = "./sol/test";
const char *TMP_FILE = "./tmp.txt";

enum program { FIFOGET = 0, FIFOSKP = 1, FIFOREV = 2, FIFOPLY = 3 };
char *programs[] = {"fifoget", "fifoskp", "fiforev", "fifoply",
                    "FIFOGET", "FIFOSKP", "FIFOREV", "FIFOPLY"};

void success(char *msg) {
    printf("%s%s%s SUCCESS ! %s%s\n", msg, GREEN, BOLD, UNBOLD, DF);
}

void fail(char *msg, int code) {
    printf("%s%s%s FAILURE !%s%s(ext code = %d)\n", msg, RED, BOLD, UNBOLD, DF,
           code);
}

int fifoxxx_test(int, char *, int, enum program);
void call_writer(char *);
int check_stdout(char *, int);
int open_tmp_file(void);
void close_del_tmp_file(int fd);
void test(enum program, char *, char *);

char error_buffer[MSG_BUFFER];
char msg_buffer[MSG_BUFFER];

int main(void) {
    if (mkfifo(FIFO_PATH, O_CREAT | 0666) == -1) {
        if (errno != EEXIST) {
            perror("fifo");
            exit(5);
        }
    }

    remove(TMP_FILE);

    test(FIFOGET, "ABCDEF034", "A\nB\nC\nD\nE\nF\n0\n3\n4\n");
    test(FIFOSKP, "A1B2C3D4Z", "A\n1\nB\n2\nC\n3\nD\n4\n");
    test(FIFOREV, "A1B2C3D4Z", "Z\n4\nD\n3\nC\n2\nB\n1\nA\n");
    test(FIFOPLY, "A1B2C3D4Z", "Z\nD\nC\nB\nA\n4+3+2+1=10\n");

    remove(FIFO_PATH);
}

void test(enum program p, char *input_writer, char *should_output) {
    int test_fd = open_tmp_file();
    printf("%s----------------------------%s\n", BOLD, UNBOLD);
    printf("%s|     TESTING %s      |%s\n", BOLD, programs[p + 4], UNBOLD);
    printf("%s----------------------------%s\n", BOLD, UNBOLD);
    strcpy(msg_buffer, "Does it return 0? ");
    int code = fifoxxx_test(SUCCESS, input_writer, test_fd, p);
    if (code == 0)
        success(msg_buffer);
    else
        fail(msg_buffer, code);
    // Check stdout
    strcpy(msg_buffer, "Is stdout correct?");
    code = check_stdout(should_output, test_fd);
    if (code == 0)
        success(msg_buffer);
    else if (code == NOT_MATCHING)
        fail(msg_buffer, code);
    else
        fail(msg_buffer, code);
    // Check return value 1
    strcpy(msg_buffer, "Does it return 1? ");
    code = fifoxxx_test(FAILURE, input_writer, test_fd, p);
    if (code == 1)
        success(msg_buffer);
    else
        fail(msg_buffer, code);
    // delete tmp.txt
    close_del_tmp_file(test_fd);
}

int fifoxxx_test(int success, char *string_input, int tmp_fd, enum program p) {
    int return_val = 0;
    int len = strlen(string_input);
    char n_reader[4];
    if (success)
        snprintf(n_reader, 4, "%d", len);
    else
        snprintf(n_reader, 4, "%d", len + 1);
    if (!fork()) call_writer(string_input);
    // save stdout fd
    int out_fd = dup(STDOUT_FILENO);
    if (out_fd == -1) {
        perror("First dup");
    }

    // Divert stdout to file
    if (dup2(tmp_fd, STDOUT_FILENO) == -1) {
        perror("Diverting the stdout");
    }
    lseek(tmp_fd, 0, SEEK_SET);
    pid_t pid = fork();
    if (pid == 0) {
        // Call to fifoxxx
        snprintf(msg_buffer, MSG_BUFFER, "./%s%s", PROGRAMS_PATH, programs[p]);
        if (execl(msg_buffer, msg_buffer, FIFO_PATH, n_reader, NULL) == -1) {
            return_val = NOT_STARTED;
        }
    } else {
        // write to fifo
        int child_stat;
        waitpid(pid, &child_stat, 0);

        // diverting back stdout
        if (dup2(out_fd, STDOUT_FILENO) == -1) {
            perror("Diverting back the stdout");
        }

        if (WIFEXITED(child_stat)) {
            return_val = WEXITSTATUS(child_stat);
        } else {
            return_val = NOT_EXITED;
        }
    }

    return return_val;
}

void call_writer(char *string_input) {
    execl(WRITER_PATH, WRITER_PATH, FIFO_PATH, string_input, NULL);
}

int check_stdout(char *should_be, int fd) {
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("[Test] Lseek tmp.txt");
        return -1;
    }
    int len = strlen(should_be);
    char c;
    for (int i = 0; i < len; i++) {
        int bytes = read(fd, &c, sizeof(char));
        if (bytes == -1) perror("[Test] Reading tmp.txt");
        if (c != should_be[i]) {
            return NOT_MATCHING;
        }
    }
    if (read(fd, &c, sizeof(char)) != 0) return EXTRA_CHAR;
    return 0;
}

int open_tmp_file(void) {
    // Open or create a temporary text file to save stdout
    int test_fd = open(TMP_FILE, O_CREAT | O_RDWR, 0666);
    if (test_fd == -1) {
        perror("Open tmp.txt");
    }
    return test_fd;
}

void close_del_tmp_file(int fd) {
    close(fd);
    remove(TMP_FILE);
}
