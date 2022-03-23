#ifndef SEM_H
#define SEM_H

/*
 * Semaphore implementation for the synchronization of POSIX threads.
 *
 * This module implements counting P/V semaphores suitable for the 
 * synchronization of POSIX threads. POSIX mutexes and condition variables 
 * shall be utilized to implement the semaphore operations.
 */

/* Opaque type of semaphore. */
typedef struct SEM SEM;

/* Creates a new semaphore.
 *
 * This function creates a new semaphore.
 *
 * Parameters:
 *
 * initVal      the initial value of the semaphore
 *
 * Returns:
 *
 * handle for the created semaphore, or NULL if an error occurred.
 */
SEM *sem_init(int initVal);

/* Destroys a semaphore and frees all associated resources.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to destroy
 *
 * Returns:
 *
 * 0 on success, negative value on error. 
 *
 * In case of an error, not all resources may have been freed, but 
 * nevertheless the semaphore handle must not be used anymore.
 */
int sem_del(SEM *sem);

/* P (wait) operation (from Dijkstra's abbreviation "prolaag", "to try to reduce").
 * 
 * Attempts to decrement the semaphore value by 1. If the semaphore value 
 * is 0, the operation blocks until a V operation increments the value and 
 * the P operation succeeds.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to decrement
 */
void P(SEM *sem);

/* V (signal) operation (from the Dutch word "vrijgave", "to release").
 *
 * Increments the semaphore value by 1 and notifies P operations that are 
 * blocked on the semaphore of the change.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to increment
 */
void V(SEM *sem); 

#endif

