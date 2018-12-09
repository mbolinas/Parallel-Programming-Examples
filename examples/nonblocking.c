#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

void MYMPI_Sendrecv();

int rank;
int nprocs;

int main(){

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  int x = 5;
  int y = 2;
  if(rank == 0){
    x = 2;
  }
  MYMPI_Sendrecv(&x, 1, MPI_INT, 1, 44, &y, 1, MPI_INT, 0, 44, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  printf("rank %d: x=%d, y=%d\n", rank, x, y);

  MPI_Finalize();

  return 0;
}

void MYMPI_Sendrecv(const void *send, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recv, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status){

  //printf("rank %d was here\n", rank);

  MPI_Request req;
  if(rank == source){
    //printf("noot\n");
    MPI_Isend(send, sendcount, sendtype, dest, sendtag, comm, &req);
    MPI_Wait(&req, MPI_STATUS_IGNORE);
    printf("abc\n");
  }
  else if(rank == dest){
    //printf("1\n");
    MPI_Irecv(recv, recvcount, recvtype, source, recvtag, comm, &req);
    MPI_Wait(&req, MPI_STATUS_IGNORE);
    printf("123\n");
  }
}
