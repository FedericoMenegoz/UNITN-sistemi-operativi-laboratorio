#include "utils.h"
#include <unistd.h> // STDERR_FILENO
#include <string.h>
#include <stdlib.h> // atoi
#include <stdio.h> // perror


void red_color(int fd) {
    write(fd, "\033[0;31m", 7);
}

void default_color(int fd) {
    write(fd, "\033[0;31m", 7);
}

void bold(int fd) {
    write(fd, "\e[1m", 4);
}

void unbold(int fd) {
    write(fd, "\e[0m", 4);
}

void perr(char * msg) {
    red_color(STDERR_FILENO);
    perror(msg);
    default_color(STDERR_FILENO);
}

int str_to_positive_int(char * ptr) {
    if (strspn(ptr, "0123456789") != strlen(ptr)) {
        return -1;
    } else {
        return atoi(ptr);
    }
}