#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[]){

  assert(argc == 3);


  
  int n = atoi(argv[1]);
  int m = atoi(argv[2]);

  int **matrix;
  matrix = malloc(sizeof(int *) * n);
  for(int i = 0; i < n; i++){
    matrix[i] = malloc(sizeof(int) * m);
  }

  //printf("?\n");
  int count = 0;
  for(int i = 0; i < n; i++){
    for(int j = 0; j < m; j++){
      matrix[i][j] = ((i * j) % m) + j + (count * ((n + 1) / m));
      //printf("%d ", matrix[i][j]);
      count = (count + 1) % (m - 1);
    }
    //printf("\n");
  }

  clock_t tic = clock();

  double total = 0;
  for(int i = 0; i < n; i++){
    double partialsum = 0;
    for(int j = 0; j < m; j++){
      partialsum = partialsum + (matrix[i][j] * matrix[i][j]); 
    }
    total = total + sqrt(partialsum);
  }

  clock_t toc = clock();

  double time_elapsed = (double) (toc - tic) / CLOCKS_PER_SEC;

  printf("total: %f (%f seconds)\n", total, time_elapsed);

  return 0;
}
