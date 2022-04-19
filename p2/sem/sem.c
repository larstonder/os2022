#include "sem.h"
#include <pthread.h>
#include <malloc.h>

typedef struct SEM {
    volatile unsigned value;
    pthread_mutex_t mutex;
    pthread_cond_t  condVar;
} SEM;

SEM *sem_init(int initVal){
    /* Implementations of POSIX semaphores generally don't allow for negative values */
    if(initVal < 0) return NULL;

    /* Allocate program memory for the semaphore */
    SEM *sem_pointer = (SEM *)malloc(sizeof(SEM));
    if(!sem_pointer) return NULL;

    /* Initialize the semaphore
     * We use a so-called "compound literal"
     * to initialize all fields using a single "=" operator */
    SEM sem = {
            .value =    initVal,
            .mutex =    PTHREAD_MUTEX_INITIALIZER,
            .condVar =  PTHREAD_COND_INITIALIZER
    };

    /* Initialize sem.mutex and sem.condVar */
    pthread_mutex_init(&sem.mutex, NULL);
    pthread_cond_init(&sem.condVar, NULL);

    /* Populate allocated memory with locally initialized fields */
    sem_pointer->value = sem.value;
    sem_pointer->mutex = sem.mutex;
    sem_pointer->condVar = sem.condVar;

    /* Return the pointer to the allocated memory
     * which contains the initialized semaphore */
    return sem_pointer;
}

int sem_del(SEM *sem){
    pthread_mutex_destroy(&sem->mutex);
    pthread_cond_destroy(&sem->condVar);

    /* frees the memory used by the appointed semaphore */
    free(sem);
    /* kinda feels like bad practice,
     * as it could cause memory fragmentation when many small semaphores are deleted,
     * but the exercise asked to free _all_ resources,
     * so it do be like that */

    return 0;
}

/* "Wait" operation */
void P(SEM *sem){
    pthread_mutex_lock(&sem->mutex);
    while(sem->value == 0) {
        pthread_cond_wait(&sem->condVar,&sem->mutex);
    }
    --(sem->value);
    pthread_mutex_unlock(&sem->mutex);
}

/* "Signal" operation */
void V(SEM *sem){
    pthread_mutex_lock(&sem->mutex);
    ++(sem->value);
    pthread_mutex_unlock(&sem->mutex);
    pthread_cond_signal(&sem->condVar);
}