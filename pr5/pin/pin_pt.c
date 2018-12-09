
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>


pthread_mutex_t mutex;
int thread_count;
int stop;
int span;
int result = 0;

void * calc_pin(void * arg){
  int *myid = (int *) arg;
  int local_ans = 0;

  for(int i = 0; i < stop; i++){
    
    if(i % thread_count == *myid){
      double tmp = sin(i) * sin(i);
      tmp = tmp * 10000.0;
      local_ans = (local_ans + (int) tmp) % 10000;
    }

  }

  pthread_mutex_lock(&mutex);
  result = (result + local_ans) % 10000;
  pthread_mutex_unlock(&mutex);

  return NULL;
}


int main(int argc, char *argv[]){
  assert(argc == 3);
  clock_t tic = clock();


  thread_count = atoi(argv[1]);
  stop = atoi(argv[2]);
  pthread_t threads[thread_count];
  int thread_ids[thread_count];
  span = stop / thread_count;

  pthread_mutex_init(&mutex, NULL);

  for(int i = 0; i < thread_count; i++){
    thread_ids[i] = i;
    pthread_create(threads + i, NULL, calc_pin, &thread_ids[i]);
  }
  for(int i = 0; i < thread_count; i++){
    pthread_join(threads[i], NULL);
  }

  clock_t toc = clock();
  double time = ((double)(toc - tic)) / CLOCKS_PER_SEC;
  
  printf("The PIN is %d\n", result);
  printf("Threads created: %d\nTime taken: %.2fs\n", thread_count, time);


  pthread_mutex_destroy(&mutex);
  return 0;
}
