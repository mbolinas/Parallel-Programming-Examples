

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


#define INTERVALS 5000000000L

int nprocs;
int myrank;

int main(int argc, char *argv[]){

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);



  long long int n = INTERVALS;
  long long int span = n / nprocs;
  long long int remainder = n % nprocs;
  long long int start;
  long long int stop;

 
  if(myrank < remainder){
    start = myrank * (span + 1);
    stop = start + span;
  }
  else{
    start = myrank * span + remainder;
    stop = start + (span - 1);
  }

  long double area = 0.0;
  long double xi;
  long i;
  for(i = start; i < stop; i++){
    xi=(1.0L/INTERVALS)*(i+0.5L);
    area += 4.0L/(INTERVALS*(1.0L+xi*xi));
  }

  if(myrank != 0){
    MPI_Send(&area, 1, MPI_LONG_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
  else{
    MPI_Status status;
    long double count;
    for(int i = 1; i < nprocs; i++){
      MPI_Recv(&count, 1, MPI_LONG_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
      area = area + count;
    }

    printf("Pi is %20.17Lf\n", area);
  }


 
  //printf("Process %d: start %lld, end %lld\n", myrank, start, stop);


  MPI_Finalize();

}
