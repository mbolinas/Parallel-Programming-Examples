
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
//#include "all2all.c"

void MY_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);

int nprocs;
int rank;

int main(){

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int *sbuf, *rbuf;
  sbuf = malloc(sizeof(int) * nprocs);
  rbuf = malloc(sizeof(int) * nprocs);
  

  
  printf("rank %d array:", rank);
  for(int i = 0; i < nprocs; i++){
    sbuf[i] = rank;
    printf(" %d", sbuf[i]);
  }
  printf("\n");
  fflush(stdout);


  
  MY_Alltoall(sbuf, 1, MPI_INT, rbuf, 1, MPI_INT, MPI_COMM_WORLD);


  
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0)
    printf("After All2all:\n");
  MPI_Barrier(MPI_COMM_WORLD);



  for(int i = 0; i < nprocs; i++){
    if(rank == i){
      printf("Rank %d array:", i);
      for(int j = 0; j < nprocs; j++){
	printf(" %d ", rbuf[j]);
      }
      printf("\n");
      fflush(stdout);
    }
  }





  MPI_Finalize();
  return 0;
}
