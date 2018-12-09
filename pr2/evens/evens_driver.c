#include "evens_lib.h"

int main(int argc, char *argv[]){

  
  
  int *arr = malloc(argc * sizeof(int));

  int count = 1;
  int currentelement = 0;
  while(count < argc){
    char *tmp = *(argv + count);
    int n = atoi(tmp);
    
    arr[currentelement] = n;
    currentelement++;
    count++;
  }
  int *evens_count = malloc(sizeof(int));

  int *evens_array;
  evens_array = find_evens(arr, argc, evens_count);
  print_array(evens_array, *evens_count);


  return 0;
}


