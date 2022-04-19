#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sem.h"

static const int N = 20;
static const int Z = 200000;
static int COUNTER = 0;

void *thread_func(void *semaphore) {
    SEM * sem = semaphore;

    P(sem);
    for(int i = 0; i < Z; i++) {
        ++COUNTER;
    }
    V(sem);
    return NULL;
}

int main(void) {
    SEM *sem = sem_init(1);

    pthread_t threads[N];
    for(int i = 0; i < N; i++) {
        pthread_create(&threads[i],NULL,thread_func,sem);
    }

    for(int i = 0; i < N; i++) {
        pthread_join(threads[i],NULL);
    }

    fprintf(stderr,"%d\n", COUNTER);
    return EXIT_SUCCESS;
}
