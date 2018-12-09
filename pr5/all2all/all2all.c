
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


void MY_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm){

  int myrank;
  int totalprocs;
  MPI_Comm_size(MPI_COMM_WORLD, &totalprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  MPI_Request *requests;
  requests = malloc(sizeof(MPI_Request) * 2 * totalprocs);



  if(sendtype == MPI_INT){
    /*
      Both Isend and Irecv require type-casting the buffer before being able to do pointer arithmetic
      Unfortunately, we can't simply do [ (sendtype *) sendbuf ] because sendtype isn't a data type
      Therefore, the only method I could find was to have a separate if statement for each MPI datatype...
     */

    for(int i = 0; i < totalprocs; i++){
      MPI_Isend(((int *) sendbuf) + (i * sendcount), sendcount, sendtype, i, 0, MPI_COMM_WORLD, &requests[i]);
    }
    for(int i = 0; i < totalprocs; i++){
      MPI_Irecv(((int *) recvbuf) + (i * recvcount), recvcount, recvtype, i, 0, MPI_COMM_WORLD, &requests[i + totalprocs]);
    }
    MPI_Waitall(2 * totalprocs, requests, NULL);


  }
  else if(sendtype == MPI_DOUBLE){

    for(int i = 0; i < totalprocs; i++){
      MPI_Isend(((double *) sendbuf) + (i * sendcount), sendcount, sendtype, i, 0, MPI_COMM_WORLD, &requests[i]);
    }
    for(int i = 0; i < totalprocs; i++){
      MPI_Irecv(((double *) recvbuf) + (i * recvcount), recvcount, recvtype, i, 0, MPI_COMM_WORLD, &requests[i + totalprocs]);
    }
    MPI_Waitall(2 * totalprocs, requests, NULL);


  }
  else{//clearly there'd be more else-if statements for more MPI_Datatypes

    for(int i = 0; i < totalprocs; i++){
      MPI_Isend(((int *) sendbuf) + (i * sendcount), sendcount, sendtype, i, 0, MPI_COMM_WORLD, &requests[i]);
    }
    for(int i = 0; i < totalprocs; i++){
      MPI_Irecv(((int *) recvbuf) + (i * recvcount), recvcount, recvtype, i, 0, MPI_COMM_WORLD, &requests[i + totalprocs]);
    }
    MPI_Waitall(2 * totalprocs, requests, NULL);


  }
}
