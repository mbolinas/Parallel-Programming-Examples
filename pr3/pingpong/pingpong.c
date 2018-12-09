



#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>



#define NPINGS 1000000

int nprocs;
int myrank;


int main(int argc, char *argv[]){
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);


  MPI_Barrier(MPI_COMM_WORLD);


  if(myrank == 0){
    //int buffer = 0;
    MPI_Status status;
    for(int pid = 1; pid < nprocs; pid++){
      clock_t tic = clock();
      for(int pnum = 0; pnum < NPINGS; pnum++){
       	MPI_Send(NULL, 0, MPI_INT, pid, 0, MPI_COMM_WORLD);
       	MPI_Recv(NULL, 0, MPI_INT, pid, 0, MPI_COMM_WORLD, &status);
      }
      clock_t toc = clock();
      double time_elapsed = (double)(toc - tic) / CLOCKS_PER_SEC;
      double ping = time_elapsed / (2 * NPINGS);
      printf("Average time to transmit between 0 and %d: %11.10f\n", pid, ping);
    }
  }
  else{
    //int buffer = 0;
    MPI_Status status;
    for(int pnum = 0; pnum < NPINGS; pnum++){
      MPI_Recv(NULL, 0, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Send(NULL, 0, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

  }


  MPI_Finalize();

}
