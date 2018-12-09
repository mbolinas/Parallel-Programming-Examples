

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int nprocs;
int rank;


int main(){
  

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  if(rank == 0){
    int n = 27;

    double arr[n];

    for(int i = 0; i < n; i++){
      arr[i] = i;
    }

    int span = n / nprocs;
    int extra = n % nprocs;

    int sendcounts[nprocs];
    sendcounts[0] = span + extra;

    for(int i = 1; i < nprocs; i++){
      sendcounts[i] = span;
    }

    int displacements[nprocs];
    for(int i = 0; i < nprocs; i++){
      displacements[i] = i * sendcounts[i];
    }

    MPI_Scatterv(arr, sendcounts, displacements, MPI_DOUBLE, recvbuf, recvcount, MPI_DOUBLE, 0, MPI_COMM_WORLD);


  }
  else{
    
  }

}
