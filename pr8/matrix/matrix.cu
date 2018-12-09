#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "book.h"

int n, m, i, j;

__global__ void l2Norm(double * m_d, int numRow, int numCol){  
  int tid = blockIdx.x;//tid = row #
  double partialsum = 0;
  for(int i = 0; i < numCol; i++){
    partialsum = partialsum + (m_d[(tid * numCol) + i] * m_d[(tid * numCol) + i]);
  }
  m_d[(tid * numCol)] = sqrt(partialsum);
}


int main(int argc, char * argv[]) {

  assert(argc==3);


  n = atoi(argv[1]);//rows
  m = atoi(argv[2]);//columns


  double *d_a;

  double *arr1 = (double *) malloc((n * m) * sizeof(*arr1));

  
  printf("test before main loops \n");
  fflush(stdout);


  //"randomly" allocate the array
  int count = 0;
  for(int i = 0; i < n * m; i++){
    arr1[i] = (i % m) + (count * ((n + 1) / m));
    //printf("%f ", arr1[i]);
    
    if(i % m == m - 1){
      //printf("\n");
    }
    
    count = (count + 1) % (m - 1);
  }

  printf("\n");

  clock_t tic = clock();

  HANDLE_ERROR(cudaMalloc( (void**) &d_a, (n * m) * sizeof(double)));
  HANDLE_ERROR(cudaMemcpy( d_a, arr1, (n * m) * sizeof(double), cudaMemcpyHostToDevice));

  l2Norm<<<n, 1>>>(d_a, n, m);

  HANDLE_ERROR(cudaMemcpy( arr1, d_a, (n * m) * sizeof(double), cudaMemcpyDeviceToHost));

  double total = 0;
  for(int i = 0; i < n * m; i = i + m){
    total = total + arr1[i];
  }
    
  clock_t toc = clock();
  double time_elapsed = (double) (toc - tic) / CLOCKS_PER_SEC;

  printf("Total: %lf (%f sec) \n", total, time_elapsed);
  fflush(stdout);

  cudaFree(d_a);
  return 0;
}
