/* Producer-consumer example. Single producer, single consumer, single datum.
 *
 * Author: Stephen F. Siegel
 * Date : 2016-oct-24
 */
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "sem.h"

int niter = 100; // number of times to loop
semaphore sem1, sem2; // signals to read and write
int buf; // the buffer
int buffersize;
int *arr;
int start = 0;
int end = 0;
pthread_mutex_t m;

void * producer(void * arg) {
  int value; // local var to store value produced

  for (int i=0; i<niter; i++) {
    value = rand(); // represents "producing" some value
    semaphore_P(&sem1); // wait for signal from consumer: safe to write
    pthread_mutex_lock(&m);
    buf = value;
    arr[end] = value;
    //end = (end + 1) % buffersize;
    //printf("(p: %d)\n", value);
    printf("Producer [pos=%d]: produced %d\n", end, value);
    end = (end + 1) % buffersize;
    pthread_mutex_unlock(&m);
    fflush(stdout);
    semaphore_V(&sem2); // signal to consumer: safe to read
  }
  return NULL;
}

void * consumer(void * arg) {
  int value; // local var to store value consumed
  
  for (int i=0; i<niter; i++) {
    semaphore_P(&sem2); // wait for signal from producer: safe to read
    pthread_mutex_lock(&m);
    value = buf;

    
    //printf("(c: %d)\n", value);
    value = arr[start];
    printf("Consumer [pos=%d]: read %d\n", start, value);
    start = (start + 1) % buffersize;
    

    fflush(stdout);
    pthread_mutex_unlock(&m);
    semaphore_V(&sem1); // signal to producer: safe to write
  }
  return NULL;
}

int main(int argc, char *argv[]) {

  assert(argc == 2);
  buffersize = atoi(argv[1]);
  arr = malloc(sizeof(int) * buffersize);
  for(int i = 0; i < buffersize; i++){
    arr[i] = 0;
  }
  
  semaphore_init(&sem1, buffersize);
  semaphore_init(&sem2, 0);
  pthread_mutex_init(&m, NULL);
  
  pthread_t producer_thread, consumer_thread;

  pthread_create(&producer_thread, NULL, producer, NULL);
  pthread_create(&consumer_thread, NULL, consumer, NULL);
  pthread_join(producer_thread, NULL);
  pthread_join(consumer_thread, NULL);
  semaphore_destroy(&sem1);
  semaphore_destroy(&sem2);
  pthread_mutex_destroy(&m);
}
