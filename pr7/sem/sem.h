/* A basic semaphore library, using Pthreads mutexes and condition variables.
 * Author: Stephen F. Siegel
 * Date: Oct 2018
 */
#include <pthread.h>

/* The opaque type semaphore */
typedef struct _semaphore {
  unsigned int val;
  pthread_mutex_t mutex;
  pthread_cond_t condition_var;
} semaphore;

/* Initializes the semaphore structure with the given value */
void semaphore_init(semaphore *s, int val);

/* Destroys the semaphore structure */
void semaphore_destroy(semaphore *s);

/* Increments s atomically, and returns the result.  Notifies
 * threads waiting for a change on s.  Returns the new value of s. */
int semaphore_V(semaphore *s);

/* Waits for s to be positive and decrements atomically.
 * Returns the new value of s */
int semaphore_P(semaphore *s);

/* Returns the value of semaphore s.  The read is atomic. */
int semaphore_val(semaphore *s);
