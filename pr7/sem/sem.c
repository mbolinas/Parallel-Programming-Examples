/* A basic semaphore library, using Pthreads mutexes and condition variables.
 * Author: Stephen F. Siegel
 * Date: Oct 2018
 */
#include "sem.h"
#include <stdlib.h>
#include <pthread.h>

/* Initializes the semaphore structure with the given value */
void semaphore_init(semaphore *s, int val) {
  s->val = val;
  pthread_mutex_init(&s->mutex, NULL);
  pthread_cond_init(&s->condition_var, NULL);
}

/* Destroys the semaphore structure */
void semaphore_destroy(semaphore *s) {
  pthread_mutex_destroy(&s->mutex);
  pthread_cond_destroy(&s->condition_var);
}

/* Increments s atomically, and returns the result.  Notifies
 * threads waiting for a change on s.  Returns the new value of s. */
int semaphore_V(semaphore *s) {
  int result;
  pthread_mutex_lock(&s->mutex);
  result = ++(s->val);
  pthread_cond_broadcast(&s->condition_var);
  pthread_mutex_unlock(&s->mutex);
  return result;
}

/* Waits for s to be positive and decrements atomically.
 * Returns the new value of s */
int semaphore_P(semaphore *s) {
  int result;
  pthread_mutex_lock(&s->mutex);
  // wait until s->val > 0...
  while (s->val == 0) pthread_cond_wait(&s->condition_var, &s->mutex);
  result = --(s->val);
  pthread_mutex_unlock(&s->mutex);
  return result;   
}

/* Returns the value of semaphore s.  The read is atomic. */
int semaphore_val(semaphore *s) {
  int result;
  pthread_mutex_lock(&s->mutex);
  result = s->val;
  pthread_mutex_unlock(&s->mutex);
  return result;
}
