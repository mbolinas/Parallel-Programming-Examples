/* diffusion1d_seq.c: sequential version of 1d diffusion.
 * The length of the rod is 1. The endpoints are frozen at 0 degrees.
 *
 * Author: Stephen F. Siegel <siegel@udel.edu>, September 2018
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <gd.h>
#include <mpi.h>
#include <stdbool.h>
#include <time.h>
#define MAXCOLORS 256
#define INITIAL_TEMP 100.0
#define PWIDTH 2       // width of 1 conceptual pixel in real pixels
#define PHEIGHT 100    // height of 1 conceptual pixel in real pixels
//#define DEBUG true
/* Global variables */
double k;                /* diffusivity constant, D*dt/(dx*dx) */
int nx;                  /* number of discrete points including endpoints */
int nsteps;              /* number of time steps */
int wstep;               /* write frame every wstep time steps */
double dx;               /* distance between two grid points: 1/(nx-1) */
double *u;               /* temperature function */
double *u_new;           /* second copy of temp. */
FILE *file;              /* file containing animated GIF */
gdImagePtr im, previm;   /* pointers to consecutive GIF images */
int *colors;             /* colors we will use */
int framecount = 0;      /* number of animation frames written */

int nprocs;
int rank;
int myspan;

void exchange_ghosts();
int *offsets;//stores offsets for each process; the starting point
int *span;//stores the size of the block for each 



double *recv_buff;
double *recv_buff_new;

/* init: initializes global variables. */
void init(int argc, char *argv[]) {
  if (argc != 6) {
    fprintf(stderr, "Usage: heat m k nx nsteps wstep filename\n");
    fprintf(stderr, "\
         k = diffusivity constant, D*dt/(dx*dx) (double)\n\
        nx = number of points in x direction, including endpoints (int)\n\
    nsteps = number of time steps (int)\n\
     wstep = write frame every this many steps (int)\n\
  filename = name of output GIF file (string)\n");
    fflush(stderr);
  }
  k = atof(argv[1]);
  nx = atoi(argv[2]);
  nsteps = atoi(argv[3]);
  wstep = atoi(argv[4]);
  file = fopen(argv[5], "wb");
  assert(file);
  printf("Running heat with k=%.3lf, nx=%d, nsteps=%d, wstep=%d, filename=%s\n", k, nx, nsteps, wstep, argv[5]);
  fflush(stdout);
  assert(k>0 && k<.5);
  assert(nx>=2);
  assert(nsteps>=1);
  assert(wstep>=0);
  dx = 1.0/(nx-1);
  u = (double*)malloc(nx*sizeof(double));
  assert(u);
  u_new = (double*)malloc(nx*sizeof(double));
  assert(u_new);
  for (int i = 0; i < nx; i++){
    u[i] = INITIAL_TEMP;
    u[0] = u_new[0] = u[nx-1] = u_new[nx-1] = 0.0;
    colors = (int*)malloc(MAXCOLORS*sizeof(int));
  }
  assert(colors);
  




  offsets = malloc(nprocs * sizeof(int));
  span = malloc(nprocs * sizeof(int));

  double span_avg = (double) (nx / nprocs);
  int remainder = (int) (nx % nprocs);

  //stores start point of each process
  for(int i = 0; i < nprocs; i++){
    offsets[i] = (int) (span_avg * i);
  }
  //stores the span of each process
  for(int i = 0; i < nprocs; i++){
    if(i < nprocs - 1){
      span[i] = offsets[i + 1] - offsets[i];
    }
    else{//can't really call offsets[i + 1] when i is max
      span[i] = nx - offsets[i];
    }
  }
}

/* write_plain: write current data to plain text file and stdout */
void write_plain(int time) {
  FILE *plain;
  char filename[100], command[100];

  sprintf(filename, "./seqout/out_%d", time);
  plain = fopen(filename, "w");
  assert(plain);
  for (int i = 0; i < nx; i++) fprintf(plain, "%8.2lf", u[i]);
  fprintf(plain, "\n");
  fclose(plain);
  sprintf(command, "cat %s", filename);
  system(command);
}


/* write_frame: add a frame to animation */
void write_frame(int time) {
  im = gdImageCreate(nx*PWIDTH,PHEIGHT);
  if (time == 0) {
    for (int j=0; j<MAXCOLORS; j++)
      colors[j] = gdImageColorAllocate(im, j, 0, MAXCOLORS-j-1);
    /* (im, j,j,j); gives gray-scale image */
    gdImageGifAnimBegin(im, file, 1, -1);
  } else {
    gdImagePaletteCopy(im, previm);
  }
  for (int i=0; i<nx; i++) {
    int color = (int)(u[i]*MAXCOLORS/INITIAL_TEMP);

    assert(color >= 0);
    if (color >= MAXCOLORS) color = MAXCOLORS-1;
    gdImageFilledRectangle(im, i*PWIDTH, 0, (i+1)*PWIDTH-1, PHEIGHT-1,
                           colors[color]);
  }
  if (time == 0) {
    gdImageGifAnimAdd(im, file, 0, 0, 0, 0, gdDisposalNone, NULL);
  } else {
    // Following is necessary due to bug in gd.
    // There must be at least one pixel difference between
    // two consecutive frames.  So I keep flipping one pixel.
    // gdImageSetPixel (gdImagePtr im, int x, int y, int color);
    gdImageSetPixel(im, 0, 0, framecount%2);
    gdImageGifAnimAdd(im, file, 0, 0, 0, 5, gdDisposalNone, previm /*NULL*/);
    gdImageDestroy(previm);
  }
  previm=im;
  im=NULL;
  framecount++;
}

/* updates u for next time step. */
void update(){
  if(nprocs > 1){
    exchange_ghosts();
  }


  int start;
  int end;
  if(rank == 0){
    start = 2;
    end = myspan + 1;
  }
  else if(rank == nprocs - 1){
    start = 1;
    end = myspan;
  }
  else{
    start = 1;
    end = myspan + 1;
  }
  for (int i = start; i < end; i++){
    recv_buff_new[i] =  recv_buff[i] + k*(recv_buff[i+1] + recv_buff[i-1] -2*recv_buff[i]);
  }
  
  //flip flop the receive buffers
  double *temp = recv_buff_new;
  recv_buff_new = recv_buff;
  recv_buff = temp;
}
void exchange_ghosts(){
  if(rank == 0){//left edge only has one ghost cell
    MPI_Sendrecv(&recv_buff[myspan], 1, MPI_DOUBLE, (rank + 1), 2, &recv_buff[myspan + 1], 1, MPI_DOUBLE, (rank + 1), 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  else if(rank < nprocs - 1){
    MPI_Sendrecv(&recv_buff[1], 1, MPI_DOUBLE, (rank - 1), 1, &recv_buff[myspan + 1], 1, MPI_DOUBLE, (rank + 1), 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Sendrecv(&recv_buff[myspan], 1, MPI_DOUBLE, (rank + 1), 2, &recv_buff[0], 1 , MPI_DOUBLE, (rank - 1), 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  else{//right edge only has one ghost cell
    MPI_Sendrecv(&recv_buff[1], 1, MPI_DOUBLE, (rank - 1), 1, &recv_buff[0], 1, MPI_DOUBLE, (rank - 1), 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
}





int main(int argc, char *argv[]) {
  clock_t tic = clock();//start the timer

  MPI_Init(&argc, &argv); 
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  init(argc, argv);


  myspan = span[rank];
  recv_buff = malloc((myspan+2) * sizeof(double));
  recv_buff_new = malloc((myspan+2) * sizeof(double));


  MPI_Scatterv(u, span, offsets, MPI_DOUBLE, &recv_buff[1], myspan, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if(rank == 0){
    write_frame(0);
  }


  for (int time = 1; time <= nsteps; time++) {
    update();
    //not sure if barrier is needed, since MPI_Gatherv should be blocking implicitly
    MPI_Barrier(MPI_COMM_WORLD);

    if (wstep != 0 && time%wstep==0) {
      MPI_Gatherv(&recv_buff[1], span[rank], MPI_DOUBLE, u, span, offsets, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      if(rank == 0){
        write_frame(time);
      }
    }
  }


  if(rank == 0){//only rank 0 finishes file IO
    gdImageDestroy(previm);
    gdImageGifAnimEnd(file);
    fclose(file);
  } 
  free(colors);
  free(u);
  free(u_new);



  if(rank == 0){
    clock_t toc = clock();
    double time_elapsed = (double) (toc - tic) / CLOCKS_PER_SEC;
    printf("Time elapsed: %f sec\n", time_elapsed);
  }
  free(recv_buff);
  free(recv_buff_new);
  free(offsets);
  free(span);

  MPI_Finalize();
  return 0;
}
