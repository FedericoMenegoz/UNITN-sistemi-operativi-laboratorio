#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define STOVES 4
#define THREADS 10

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define DF "\033[0m"

pthread_mutex_t stoveMutex[STOVES];
int stoveFuel[STOVES] = { 100, 100, 100, 100 };

void * cook(void *args) {
    int id = *(int*) args;
    int cooked = 0;
    int gasNeeded = rand() % 50 + 1;
    for(int i = 0; i < STOVES && !cooked; i++) {
        if (pthread_mutex_trylock(&stoveMutex[i]) == 0) {
            if (stoveFuel[i] - gasNeeded > 0) {
                stoveFuel[i] -= gasNeeded;
                printf("Cook %d cooked using %d gas. Gas left on stove %d is %d\n", id, gasNeeded, i, stoveFuel[i]);
                cooked++;
            }
            pthread_mutex_unlock(&stoveMutex[i]);
        } else {
            if (i == 3) {
                usleep(500000);
                i = 0;
            }
        }
    }
    if (!cooked) {
        printf("%sCook %d went home as did not find a free stove or gas has ran out.%s\n", RED, id, DF);
    } else {
        printf("%sCook %d cooked %d times...%s\n", GREEN, id, cooked, DF);
    }
    return NULL;
}

int main(void){
    srand(time(NULL));
    pthread_t t[THREADS];
    int ids[THREADS];
    for (int i = 0; i < STOVES; i++) {
        pthread_mutex_init(&stoveMutex[i], NULL);
    }

    for (int i = 0; i < THREADS; i++) {
        ids[i] = i;
        if (pthread_create(&t[i], NULL, cook, &ids[i]) != 0) {
            perror("Create thread.");
            exit(i);
        }
    }

    for (int i = 0; i < THREADS; i++) {
        if (pthread_join(t[i], NULL) != 0) {
            perror("Joining thread.");
            exit(i);
        }
    }


    for (int i = 0; i < STOVES; i++) {
        pthread_mutex_destroy(&stoveMutex[i]);
        printf("Stove %d - Gas %d\n", i, stoveFuel[i]);
    }
    return 0;
}