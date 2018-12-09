/* wave1d.c: 1d-wave equation simulation.
 *
 * Author: Stephen F. Siegel
 *
 * Reads configuration file for parameters and produces animated GIF
 * of wave.  
 *
 * To keep things simple, the unit of distance
 * is 1 pixel = delta_x.  The unit of time is 1 time step = delta_t.
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <gd.h>

#define SQR(x) ((x)*(x))

/* Global variables */

int nx = -1;              /* number of discrete points including endpoints */
double c = -1;            /* physical constant to do with string */
int height_init = -1;     /* max amplitude in initial state */
int width_init = -1;      /* width of initial pulse */
int nsteps = -1;          /* number of time steps */
int wstep = -1;           /* write frame every this many time steps */
FILE *file;               /* file containing animated GIF */
gdImagePtr im, previm;    /* pointers to consecutive GIF images */
int white, black, trans;  /* colors used */
int ny;                   /* number of pixels in y direction */
double k;                 /* c*c */
/* The following give the amplitude at each x-coordinate in
 * 3 consecutive time steps.  Each has length nx.  */
double *u_prev, *u_curr, *u_next; 

void quit() {
  printf("Input file must have format:\n\n");
  printf("nx = <INTEGER>\n");
  printf("c = <DOUBLE>\n");
  printf("height_init = <INTEGER>\n");
  printf("width_init = <INTEGER>\n");
  printf("nsteps = <INTEGER>\n");
  printf("wstep = <INTEGER>\n");
  fflush(stdout);
  exit(1);
}

void readint(FILE *file, char *keyword, int *ptr) {
  char buf[101];
  int value;
  int returnval;

  returnval = fscanf(file, "%100s", buf);
  if (returnval != 1) quit();
  if (strcmp(keyword, buf) != 0) quit();
  returnval = fscanf(file, "%10s", buf);
  if (returnval != 1) quit();
  if (strcmp("=", buf) != 0) quit();
  returnval = fscanf(file, "%d", ptr);
  if (returnval != 1) quit();
}

void readdouble(FILE *file, char *keyword, double *ptr) {
  char buf[101];
  int value;
  int returnval;

  returnval = fscanf(file, "%100s", buf);
  if (returnval != 1) quit();
  if (strcmp(keyword, buf) != 0) quit();
  returnval = fscanf(file, "%10s", buf);
  if (returnval != 1) quit();
  if (strcmp("=", buf) != 0) quit();
  returnval = fscanf(file, "%lf", ptr);
  if (returnval != 1) quit();
}

/* init: initializes global variables.  reads parameters from infile.
 * Opens GIF output file.  Initializes data using a smooth function
 * with compact support. */
void init(char* infilename, char* outfilename) {
  char keyword[101];
  FILE *infile = fopen(infilename, "r");
  int i;
  double e = exp(1.0);

  assert(infile);
  readint(infile, "nx", &nx);
  readdouble(infile, "c", &c);
  readint(infile, "height_init", &height_init);
  readint(infile, "width_init", &width_init);
  readint(infile, "nsteps", &nsteps);
  readint(infile, "wstep", &wstep);
  printf("Wave1d with nx=%d, c=%f, height_init=%d, width_init=%d, \
nsteps=%d, wstep=%d\n",
	 nx, c, height_init, width_init, nsteps, wstep);
  fflush(stdout);
  assert(nx>=2);
  assert(width_init < nx);
  assert(c>0);
  assert(nsteps >= 1);
  assert(wstep >= 0 && wstep <=nsteps);
  ny =  2*(height_init+1)+1;
  u_prev = (double*)malloc(nx*sizeof(double));
  u_curr = (double*)malloc(nx*sizeof(double));
  u_next = (double*)malloc(nx*sizeof(double));
  assert(u_prev);
  assert(u_curr);
  assert(u_next);
  for (i = 0; i < nx; i++) {
    if (i == 0 || i >= width_init) {
      u_prev[i] = 0.0;
    } else {
      u_prev[i] = height_init*e*
	exp(-1.0/(1-SQR(2.0*(i-width_init/2.0)/width_init)));
    }
    u_curr[i] = u_prev[i];
  }
  fclose(infile);
  assert(outfilename);
  file = fopen(outfilename, "wb");
  assert(file);
  k = c*c;
}

/* write_plain: write current data to plain text file and stdout.
 * Used for debugging. */
void write_plain(int time) {
  FILE *plain;
  char filename[50];
  char command[50];
  int i,j;
  
  sprintf(filename, "./seqout/out_%d", time);
  plain = fopen(filename, "w");
  assert(plain);
  for (i = 0; i < nx; i++) fprintf(plain, "%8.2f", u_curr[i]);
  fprintf(plain, "\n");
  fclose(plain);
  sprintf(command, "cat %s", filename);
  system(command);
}

/* write_frame: add a frame to animation */
void write_frame(int time) {
  int i, j, y_prev;
  
  im = gdImageCreate(nx, ny);
  if (time == 0) {
    white = gdImageColorAllocate(im, 255, 255, 255);
    /* Allocate drawing color */
    black = gdImageColorAllocate(im, 0, 0, 0);
    /* Allocate transparent color for animation compression */
    trans = gdImageColorAllocate(im, 1, 1, 1);
  } else {
    /* Allocate background to make it white */
    (void)gdImageColorAllocate(im, 255, 255, 255);
    gdImagePaletteCopy(im, previm);
  }
  /* Need to make sure at least one pixel differs from previous
   * slide or GD fails due to defect... */
  gdImageSetPixel(im, time%nx, ny-1, black);
  for (i=0; i<nx; i++) {
    int y = height_init+1-u_curr[i];

    if (i>0)
      gdImageLine(im, i-1, y_prev, i, y, black);
    y_prev = y;
  }
  if (time == 0) {
    gdImageGifAnimBegin(im, file, 1, 3);
    /* void gdImageGifAnimAdd(gdImagePtr im, FILE *out,
     *    int LocalCM, int LeftOfs, int TopOfs, int Delay,
     *    int Disposal, gdImagePtr previm) 
     */
    gdImageGifAnimAdd(im, file, 0, 0, 0, 0, 1, NULL);
  } else {
    gdImageColorTransparent(im, trans);
    gdImageGifAnimAdd(im, file, 0, 0, 0, 0, 1,  previm);
    gdImageDestroy(previm);
  }
  previm=im;
  im=NULL;
#ifdef DEBUG
  write_plain(time);
#endif
}

/* updates u for next time step. */
void update() {
  int i;
  double *tmp;

  for (i = 1; i < nx-1; i++)
    u_next[i] = 2.0*u_curr[i] - u_prev[i] +
      k*(u_curr[i+1] + u_curr[i-1] - 2.0*u_curr[i]);
  tmp = u_prev;
  u_prev = u_curr;
  u_curr = u_next;
  u_next = tmp;
}

/* main: executes simulation, creates simulation.
 * Arg 1: filename for configuration file
 * Arg 2: output filename for animated gif (.gif extension)
 * Note: if the configuration specifies wstep=0, no output
 * is created (i.e., an empty file).
 */
int main(int argc, char *argv[]) {
  int iter;

  assert(argc==3);
  init(argv[1], argv[2]);
  if (wstep!=0) write_frame(0);
  for (iter = 1; iter <= nsteps; iter++) {
    update();
    if (wstep!=0 && iter%wstep==0) write_frame(iter);
  }
  if (wstep != 0) {
    gdImageDestroy(previm);
    gdImageGifAnimEnd(file);
  }
  fclose(file);
  free(u_prev);
  free(u_curr);
  free(u_next);
}
