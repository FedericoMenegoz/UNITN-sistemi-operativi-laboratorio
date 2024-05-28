#include <pthread.h>
#include <stdio.h>

#define MAX_THREADS 10

int counter = 0;
pthread_mutex_t lock;
pthread_t threads[MAX_THREADS];
int id [MAX_THREADS];

void * increase_counter(__attribute__((unused)) void * arg) {
    int i = *(int *) arg;
    pthread_mutex_lock(&lock);
    counter++;
    printf("Thread %d -> counter %d\n", i, counter);
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(void) {
    pthread_mutex_init(&lock, NULL);
    for (int i = 0; i < MAX_THREADS; i++) {
        id[i] = i;
        pthread_create(&threads[i], NULL, increase_counter, &id[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&lock);

    printf("Counter is %d\n", counter);
    return 0;
}
