//@Marc Bolinas

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include <time.h>

int nprocs;
int myrank;
int first;
int n_local;



int main(int argc, char *argv[]){
  clock_t tic = clock();//start the timer
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  
  assert(argc == 2);


  int n = atoi(argv[1]);
  int span = n / nprocs;//how big each block is for each process
  int remainder = n % nprocs;
  int start;
  int stop;


  //This distributes all integer values from 0 to n evenly across all processes
  if(myrank < remainder){
    start = myrank * (span + 1);
    stop = start + span;
  }
  else{
    start = myrank * span + remainder;
    stop = start + (span - 1);
  }




  int result = 0;
  for(int i = start; i <= stop; ++i){

    double d = i + 0.0;//type casting int to double
    double tmp = sin(d);
    double tmp2 = tmp*tmp;
    int z = (int)(tmp2*10000.0);
    result = (result + z) % 10000;
  }


  if(myrank != 0){
    MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);    
  }
  else{
    MPI_Status status;
    int count;
    for(int i = 1; i < nprocs; i++){
      MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
      result = (result + count) % 10000;
    }
    clock_t toc = clock();
    double time_elapsed = (double)(toc - tic) / CLOCKS_PER_SEC;
    printf("The PIN is %d (nprocs = %d, time = %.2f sec.)\n", result, nprocs, time_elapsed);
    printf("Result: %d\n", result);

  }

  MPI_Finalize();
}
