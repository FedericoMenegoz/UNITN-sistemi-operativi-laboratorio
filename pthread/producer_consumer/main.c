#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define LEN 20
#define THREADS 3
#define RED "\033[0;31m"
#define BLUE "\033[0;32m"
#define DF "\033[0m"

struct circular_buffer {
    char text[LEN];
    size_t size;
    short index;
} bf;

pthread_mutex_t lock;


void * producer(void * args) {
    char c = *(char *) args;
    for (int i = 0; i < LEN;) {
        pthread_mutex_lock(&lock);
        if (bf.size > 0) {
            bf.text[bf.index] = c;
            bf.index = (bf.index + 1) % LEN;
            bf.size--;
            i++;
        }
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

void * consumer(__attribute__((unused)) void * args) {
    for (int i = 0; i < 2*LEN;) {
        pthread_mutex_lock(&lock);
        if (bf.size < LEN) {

            bf.index = (bf.index + LEN - 1) % LEN; 
            if (bf.text[bf.index] == 'o') {
                printf("%s%c %s", RED, bf.text[bf.index], DF);
            } else {
                printf("%s%c %s", BLUE, bf.text[bf.index], DF);
            }
            i++;
            bf.size++;
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}


int main(void) {
    pthread_t t[THREADS];
    bf.size = LEN;
    bf.index = 0;
    char o = 'o', x = 'x';
    pthread_mutex_init(&lock, NULL);
    pthread_create(&t[0], NULL, producer, &o);        
    pthread_create(&t[1], NULL, producer, &x);        
    pthread_create(&t[2], NULL, consumer, NULL);

    for(int i = 0; i < THREADS; i ++) {
        pthread_join(t[i], NULL);
    }        
    return 0;
}
