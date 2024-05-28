#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"
#define DF "\033[0m"

#define THRESH 15
#define TIMES 20
int counter = 0;
struct timespec tim1 = {
    .tv_sec = 0,
    .tv_nsec = 500000000
}, tim2;

pthread_mutex_t lock_counter;
pthread_cond_t cond_counter;

void *threshold_reached(void *args) {
    int id = *(int *)args;
    pthread_mutex_lock(&lock_counter);
    while(counter < THRESH) {
        pthread_cond_wait(&cond_counter, &lock_counter);
    }
    pthread_mutex_unlock(&lock_counter);
    printf("[%d] Threshold reached.\n", id);
    return NULL;
}

void *increment_and_tell(void *args) {
    int id = *(int *)args;
    for (int i = 0; i < TIMES; i++) {
        pthread_mutex_lock(&lock_counter);
        if (id == 2) {
            printf("%s[%d] %d%s\n",BLUE, id, ++counter, DF);
        } else {
            printf("%s[%d] %d%s\n",GREEN, id, ++counter, DF);
        }
        if (counter > THRESH) {
            pthread_cond_signal(&cond_counter);
        }
        pthread_mutex_unlock(&lock_counter);
        nanosleep(&tim1, &tim2);
    }
    return NULL;
}

int main(void) {


    
    pthread_t th[3];
    int id_0 = 0, id_1 = 1, id_2 = 2;

    pthread_mutex_init(&lock_counter, NULL);
    pthread_cond_init(&cond_counter, NULL);
    if (pthread_create(&th[0], NULL, threshold_reached, &id_0) != 0) {
        perror("pthread threshold");
        return 1;
    }
    if (pthread_create(&th[1], NULL, increment_and_tell, &id_1) != 0) {
        perror("pthread threshold");
        return 2;
    }
    if (pthread_create(&th[2], NULL, increment_and_tell, &id_2) != 0) {
        perror("pthread threshold");
        return 3;
    }

    for (int i = 0; i < 3; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i);
            perror("join");
        }
    }

    pthread_mutex_destroy(&lock_counter);
    pthread_cond_destroy(&cond_counter);
    return 0;
}
