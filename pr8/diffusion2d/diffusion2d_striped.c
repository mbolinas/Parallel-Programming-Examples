/* diffusion2d.c: parallel row-distributed version of 2d diffusion.
 * The length of the side of the square is 1. Initially entire square
 * is 100 degrees, but edges are held at 0 degrees.
 *
 * Author: Stephen F. Siegel <siegel@cis.udel.edu>, February, 2009.
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>
#include <gd.h>
#define MAXCOLORS 256

/* Constants: the following should be defined at compilation:
 *
 *       M = initial temperature at center
 *  NSTEPS = number of time steps
 *   WSTEP = write frame every this many steps
 *      NX = number of points in x direction, including endpoints
 *       K = D*dt/(dx*dx)
 * 
 */

/* Global variables */
int nx = NX;              /* number of discrete points including endpoints
                             on one side of the square. */
int ny = NX;              /* it's a square */
double m = M;             /* initial temperature of center of square */
double k = K;             /* D*dt/(dx*dx) */
int nsteps = NSTEPS;      /* number of time steps */
double **u[2];            /* temperature function */
double *u_storage[2];     /* storage for u */
FILE *file;               /* file containing animated GIF */
gdImagePtr im, previm;    /* pointers to consecutive GIF images */
int *colors;              /* colors we will use */
const int root = 0;       /* rank of root node */
int nprocs;               /* number of processes */
int myRank;               /* rank of this process */
int down, up;             /* ranks of lower/upper neighbors */
int first;                /* global index for local index 0 */
int numRows;              /* number of rows of u owned by this process
			     (excl. ghosts) */
double *writeBuf;         /* buffer for use by root, holding one row */
int framecount = 0;       /* number of animation frames written */


/* Functions for dealing with row-distribution. 
 * ny rows are distributed over nprocs processes. */

/* The global index of the first row owned by the process of the given rank. */
int first_for_rank(int rank) {
  return (rank * ny) / nprocs;
}

/* The number of rows owned by the process of the given rank. */
int num_rows_for_rank(int rank) {
  return first_for_rank(rank+1) - first_for_rank(rank);
}

/* The rank of the process that owns the row with the given global_index.
 * The global_index must satisfy 0 <= global_index <= ny-1 */
int owner(int global_index) {
  return (nprocs*(global_index+1)-1)/ny;
}

/* The local index of the row with the given global index.  The result
 * will satisfy 0 <= local_index <= numRows-1, where numRows is
 * num_rows_for_rank(owner(global_index)). */
int local_index(int global_index) {
  return global_index - first_for_rank(owner(global_index));
}


/* Sets top and bottom of block (including corners)
 * to zero on first and last process.
 * Sets side edges of main block (not ghost rows) to 0.
 */
void zero_out_top_bottom_and_sides(int t) {
  int i,j;

#if BOUNDARY == 2
  for (i=1; i<=numRows; i++) {
    u[t][i][0] = 0.0;
    u[t][i][nx-1] = m;
  }
  if (myRank == owner(0)) {
    for (j=0; j<nx-1; j++) u[t][1][j] = 0.0;
    u[t][1][nx-1] = m/2;
  }
  if (myRank == owner(ny-1)) {
    for (j=1; j<nx; j++) u[t][numRows][j] = m;
    u[t][numRows][0] = m/2;
  }
#else
  if (myRank == owner(0)) {
    for (j=0; j<nx; j++) u[t][1][j] = 0.0;
  }
  if (myRank == owner(ny-1)) {
    for (j=0; j<nx; j++) u[t][numRows][j] = 0.0;
  }
  for (i=1; i<=numRows; i++) {
    u[t][i][0] = u[t][i][nx-1] = 0.0;
  }
#endif
}

/* init: initializes global variables. */
void init() {
  int i, j, t;

  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  if (myRank == root) {
    printf("Diffusion2d with k=%f, M=%f, nx=%d, nsteps=%d\n",
	   k, m, nx, nsteps);
    fflush(stdout);
    file = fopen("./parout/out.gif", "wb");
    assert(file);
    colors = (int*)malloc(MAXCOLORS*sizeof(int));
    assert(colors);
    writeBuf = (double*)malloc(nx*sizeof(double));
    assert(writeBuf);
  }
  assert(k>0 && k<.5);
  assert(m>=0);
  assert(nx>=2);
  assert(nsteps>=1);
  first = first_for_rank(myRank);
  numRows = num_rows_for_rank(myRank);
  if (first == 0 || numRows == 0)
    down = MPI_PROC_NULL;
  else
    down = owner(first-1);
  if (first+numRows >= ny || numRows == 0)
    up = MPI_PROC_NULL;
  else
    up = owner(first+numRows);
  /* u[t] has two extra rows for ghost cells...*/
  for (t=0; t<2; t++) {
    u_storage[t] = (double*)malloc((numRows+2)*nx*sizeof(double));
    assert(u_storage[t]);
    u[t] = (double**)malloc((numRows+2)*sizeof(double*));
    assert(u[t]);
    /* set up all row pointers including ghost rows */
    for (i=0; i<numRows+2; i++) {
      u[t][i]=&u_storage[t][i*nx];
    }
    /* initialize non-ghost rows */
    for (i=1; i<=numRows; i++)
      for (j=0; j<nx; j++)
	u[t][i][j] = m;
    zero_out_top_bottom_and_sides(t);
  }
}

/* write_plain: write current data to plain text file and stdout.
 * Write from top to bottom. */
void write_plain(int time) {
  if (myRank != root) {
    int i;

    for (i=numRows; i>=1; i--)
      MPI_Send(u[time%2][i], nx, MPI_DOUBLE, root, 0, MPI_COMM_WORLD);
  } else {
    FILE *plain;
    char filename[50];
    char command[50];
    int i, j, global_row;
  
    sprintf(filename, "./parout/out_%d", time);
    plain = fopen(filename, "w");
    assert(plain);
    for (global_row = ny-1; global_row >= 0; global_row--) {
      int source = owner(global_row);
      double *data;

      if (source != root) {
	MPI_Recv(writeBuf, nx, MPI_DOUBLE, source, 0, MPI_COMM_WORLD,
		 MPI_STATUS_IGNORE);
	data = writeBuf;
      } else {
	data = u[time%2][local_index(global_row)+1];
      }
      for (j=0; j<nx; j++) fprintf(plain, "%8.2f", data[j]);
      fprintf(plain, "\n");
    }
    fprintf(plain, "\n");
    fclose(plain);
    sprintf(command, "cat %s", filename);
    system(command);
  }
}

/* write_frame: add a frame to animation */
void write_frame(int time) {
  if (myRank != root) {
    int i;
    
    for (i = 1; i <= numRows; i++)
      MPI_Send(u[time%2][i], nx, MPI_DOUBLE, root, 0, MPI_COMM_WORLD);
  } else {
    int j, global_row, source;
    double *data;

    im = gdImageCreate(nx*PWIDTH,ny*PWIDTH);
    if (time == 0) {
      for (j=0; j<MAXCOLORS; j++)
	colors[j] = gdImageColorAllocate (im, j, 0, MAXCOLORS-j-1);
      /* (im, j,j,j); gives gray-scale image */
      gdImageGifAnimBegin(im, file, 1, -1);
    } else {
      gdImagePaletteCopy(im, previm);
    }
    for (global_row = 0; global_row < ny; global_row++) {
      int source = owner(global_row);
      double *data;

      if (source != root) {
	MPI_Recv(writeBuf, nx, MPI_DOUBLE, source, 0, MPI_COMM_WORLD,
		 MPI_STATUS_IGNORE);
	data = writeBuf;
      } else {
	data = u[time%2][local_index(global_row)+1];
      }
      for (j = 0; j < nx; j++) {
        int color = (int)(data[j]*MAXCOLORS/M);

        assert(color >= 0);
        if (color >= MAXCOLORS) color = MAXCOLORS-1;
        gdImageFilledRectangle(im, j*PWIDTH, global_row*PWIDTH,
			       (j+1)*PWIDTH-1, (global_row+1)*PWIDTH-1,
			       colors[color]);
      }
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
  }
#ifdef DEBUG
  write_plain(time);
#endif
  framecount++;  
}

void exchange_ghost_cells(int time) {
  int t = time%2;

  MPI_Sendrecv(u[t][1], nx, MPI_DOUBLE, down, 0,
               u[t][numRows+1], nx, MPI_DOUBLE, up, 0,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  MPI_Sendrecv(u[t][numRows], nx, MPI_DOUBLE, up, 0,
               u[t][0], nx, MPI_DOUBLE, down, 0,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

/* updates u for next time step. */
void update(int time) {
  int i, j;
  int old = time%2;
  int new = 1-old;

  for (i=1; i<=numRows; i++) {
    for (j=1; j<nx-1; j++){
      u[new][i][j] =  u[old][i][j]
	+ k*(u[old][i+1][j] + u[old][i-1][j]
	     + u[old][i][j+1] + u[old][i][j-1] - 4.0*u[old][i][j]);
    }
  }
  zero_out_top_bottom_and_sides(new);
}

/* main: executes simulation, creates one output file for each time
 * step */
int main(int argc, char *argv[]) {
  int iter, t;

  MPI_Init(&argc, &argv);
  init();
  write_frame(0);
  for (iter = 1; iter <= nsteps; iter++) {
    exchange_ghost_cells(iter);
    update(iter);
    if (WSTEP!=0 && iter%WSTEP==0) write_frame(iter);
  }
  if (myRank == root) {
    gdImageDestroy(previm);
    gdImageGifAnimEnd(file);
    fclose(file);
    free(colors);
    for (t=0; t<2; t++) {
      free(u_storage[t]);
      free(u[t]);
    }
    free(writeBuf);
  }
  MPI_Finalize();
}
