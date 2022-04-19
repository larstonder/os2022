#include <malloc.h>
#include <stdbool.h>
#include "bbuffer.h"
#include "../sem/sem.h"

typedef struct BNDBUF {
    int *values;
    int head, tail;
    unsigned int size;
    SEM *mutex, *number_of_empty_values, *number_of_entries;
} BNDBUF;

BNDBUF *bb_init(unsigned int size){
    if(size <= 0) return NULL;

    /* Allocate program memory for the Bounded Buffer */
    BNDBUF *bb = (BNDBUF *)malloc(sizeof(BNDBUF));
    if(!bb) return NULL;

    /* Populate allocated memory with locally initialized fields */
    bb->size = size;
    /* In this implementation, it is assumed that "size" is the max amount of values the buffer can hold.
     * As every value of the buffer is represented by a single int, the memory allocated to store its values should be:
     * the size of an int times the amount of values to be held */
    bb->values = malloc(sizeof(int) * bb->size);
    bb->head = 0;
    bb->tail = 0;
    bb->number_of_empty_values = sem_init((int) bb->size);
    bb->number_of_entries = sem_init(0);
    bb->mutex = sem_init(1);

    /* Return the pointer to the allocated memory
     * which contains the initialized semaphore */
    return bb;
}

bool buffer_is_empty(BNDBUF* bb) {
    /* Distance between head and tail is 0 */
    return ((bb->head - bb->tail) == 0);
}

bool buffer_is_full(BNDBUF* bb) {
    /* Distance between head and tail is as large as the size of the buffer */
    return ((bb->head - bb->tail) == (bb->size));
}

void bb_del(BNDBUF *bb){
    free(bb->values);
    sem_del(bb->mutex);
    sem_del(bb->number_of_entries);
    sem_del(bb->number_of_empty_values);
    free(bb);
}

int bb_get(BNDBUF *bb){
    int result;

    P(bb->number_of_entries);
    P(bb->mutex); // Mutex lock

    while(buffer_is_empty(bb)) {} // BLOCK

    unsigned index = bb->tail % bb->size;
    result = bb->values[index];
    ++(bb->tail);

    V(bb->mutex); // Mutex unlock
    V(bb->number_of_empty_values);

    return result;
}

void bb_add(BNDBUF *bb, int fd){
    P(bb->number_of_empty_values);
    P(bb->mutex); // Mutex lock

    while(buffer_is_full(bb)) {} // BLOCK

    unsigned index = bb->head % bb->size;
    bb->values[index] = fd;
    ++(bb->head);

    V(bb->mutex); // Mutex unlock
    V(bb->number_of_entries);
}