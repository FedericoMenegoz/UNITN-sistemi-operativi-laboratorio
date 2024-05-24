#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "lib.c"

void sorted_insert(char c, int size, char * array);

int main(int argc, char* argv[]) {
    int err, fd;
    int n = check_parse_args(argc, argv, &err);

    if (n < 0) {
        exit(err);
    }
    
    fd = open_fifo(argv[1], &err, O_RDONLY);
    if (fd == -1) {
        exit(err);
    }

    err = fifo_to_buffer(fd, &n);

    char letters [MAX_BUFFER];
    char numbers [MAX_BUFFER];
    int l_count = 0, n_count = 0;
    for (int i = 0; i < n; i++) {
        if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
            sorted_insert(buffer[i], l_count++, letters);
        } else if (buffer[i] >= '0' && buffer[i] <= '9') {
            numbers[n_count++] = buffer[i];
        } else {
            printf("%sError: invalid char %c%s\n", RED, buffer[i], DF);
        }
    }

    for (int i = 0; i < l_count; i++) {
        printf("%c\n", letters[i]);
    }
    int sum = 0;
    for (int i = 0; i < n_count; i++) {
        printf("%c", numbers[i]);
        if (i < n_count - 1) printf("+");
        sum += convert_from_char(numbers[i]);
    }
    printf("=%d\n", sum);
    close(fd);
    return err;
}


void sorted_insert(char c, int size, char * array) {
    int i = 0; 
    while (c < array[i]) i++;

    if (i == size) {
        array[i] = c;
    } else {
        while (i <= size) {
            char tmp = array[i];
            array[i] = c;
            c = tmp;
            i++;
        }
    }
}

