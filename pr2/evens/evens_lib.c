#include <stdio.h>
#include "evens_lib.h"

int *find_evens(int *p, int n, int *num_evens){
  int count = 0;
  int offset = 0;
  while(offset < n - 1){
    if(*(p + offset) % 2 == 0){
      count++;
    }
    offset++;
  }

  if(count == 0){
    return NULL;
  }


  int *evenarray = malloc(count * sizeof(int));

  *num_evens = count;
  int current_element = 0;
  offset = 0;
  while(offset < n){
    if(*(p + offset) % 2 == 0){
      *(evenarray + current_element) = *(p + offset);
      current_element++;
    }
    offset++;
  }

  return evenarray;
  
}

void print_array(int *p, int n){
  printf("The even numbers are: ");
  for (int i = 0; i < n; i++)
    printf("%d ", p[i]);
  printf("\n");
  fflush(stdout);
}
