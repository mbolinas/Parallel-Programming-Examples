#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>


#ifndef MAX_DEPOSIT
#define MAX_DEPOSIT 10.00
#endif
#ifndef MAX_WITHDRAW
#define MAX_WITHDRAW 20.00
#endif
#ifndef INTEREST_RATE
#define INTEREST_RATE 0.10
#endif
#ifndef NSTEPS
#define NSTEPS 10
#endif

double balance;
int terminate = 0;

pthread_mutex_t mutex;
pthread_cond_t cond;

void * deposit(void * arg){
  int id = *(int *) arg;
  printf("my id = %d\n", id);
  while(terminate == 0){
    pthread_mutex_lock(&mutex);
    double span = RAND_MAX / MAX_DEPOSIT;
    double tmp = rand() / span;
    balance = balance + tmp;
    printf("Depositor %d deposited %.2f to account %d. New balance: %.2f\n", id, tmp, 1, balance);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    //sleep(1);
  }
  return NULL;
}

void * withdraw(void * arg){
  int id = *(int *) arg;
  while(terminate == 0){
    pthread_mutex_lock(&mutex);
    double span = RAND_MAX / MAX_WITHDRAW;
    double tmp = rand() / span;
    while(tmp > balance && terminate == 0){
      printf("Withdrawer %d: WAITING to withdraw %.2f from account %d. Balance=%.2f\n", id, tmp, 1, balance);
      pthread_cond_wait(&cond, &mutex);
    }
    if(terminate == 1)
      tmp = 0.0;
    balance = balance - tmp;
    printf("Withdrawer %d withdrew %.2f from account %d. New balance: %.2f\n", id, tmp, 1, balance);

    pthread_mutex_unlock(&mutex);
    //sleep(1);
  }
  return NULL;
}

void * interest(void * arg){
  for(int i = 0; i < NSTEPS; i++){
    pthread_mutex_lock(&mutex);

    balance = balance + (balance * INTEREST_RATE);
    printf("Interest in the amount of %.2f has been applied.  New balance: %.2f\n", balance * INTEREST_RATE, balance);
    pthread_cond_broadcast(&cond);

    pthread_mutex_unlock(&mutex);
    sleep(1);
  }
  terminate = 1;
  return NULL;
}


int main(int argc, char *argv[]){

  assert(argc == 4);

  balance = atof(argv[1]);
  int num_deposit = atoi(argv[2]);
  int num_withdraw = atoi(argv[3]);

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
  pthread_t *depositers;
  pthread_t *withdrawers;
  int deposit_id[num_deposit];
  int withdraw_id[num_withdraw];


  depositers = malloc(sizeof(pthread_t) * num_deposit);
  withdrawers = malloc(sizeof(pthread_t) * num_withdraw);

  pthread_t interest_t;
  pthread_create(&interest_t, NULL, interest, NULL);

  for(int i = 0; i < num_deposit; i++){
    pthread_t tmp;
    deposit_id[i] = i;
    pthread_create(&tmp, NULL, deposit, &deposit_id[i]);
    depositers[i] = tmp;
  }
  for(int i = 0; i < num_withdraw; i++){
    pthread_t tmp;
    withdraw_id[i] = i;
    pthread_create(&tmp, NULL, withdraw, &withdraw_id[i]);
    withdrawers[i] = tmp;
  }

  pthread_join(interest_t, NULL);
  for(int i = 0; i < num_deposit; i++)
    pthread_join(depositers[i], NULL);
  pthread_cond_broadcast(&cond);
  for(int i = 0; i < num_withdraw; i++)
    pthread_join(withdrawers[i], NULL);

  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mutex);
  return 0;
}
