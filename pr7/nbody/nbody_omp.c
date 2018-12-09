//Sequential: 58.447
//Parallel: 46.807
//Speedup: 25%

/* Name: nbody_seq.c: sequential 2-d nbody simulation
 * Author: Stephen Siegel
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "gd.h"
#define MAXCOLORS 254
#define PWIDTH 1

/* There is one structure of this type for each "body" in the
 * simulation.  All of the attributes and the current state of the
 * body are recorded in this structure. */
typedef struct BodyStruct {
  double mass;     /* mass of body */
  int color;       /* color used to draw this body */
  int size;        /* diameter of body in pixels */
  double x;        /* x position */
  double y;        /* y position */
  double vx;       /* velocity, x-direction */
  double vy;       /* velocity, y-direction */
} Body;

/* Global variables */
double x_min;        /* coord of left edge of universe */
double x_max;        /* coord of right edge of universe */
double y_min;        /* coord of bottom edge of universe */
double y_max;        /* coord of top edge of universe */
double univ_x;       /* x_max-x_min */
double univ_y;       /* y_max-y_min */
int nx;              /* width of movie window (pixels) */
int ny;              /* height of movie window (pixels) */
int numBodies;       /* number of bodies */
double K;            /* single constant encoding G, grid spacing, etc. */
int nsteps;          /* number of time steps */
int period;          /* number of times steps beween movie frames */
FILE *gif;           /* file containing animated GIF */
gdImagePtr im,       /* pointers to consecutive GIF images */
  previm;  
int *colors;         /* colors we will use */
Body *bodies,        /* two copies of main data structure: list of bodies */
  *bodies_new;
int byteCount = 0;
int framecount = 0;  /* number of frames added to animation */

int threadcount;

void* my_malloc(int numBytes) {
  void *result = malloc(numBytes);

  assert(result);
  byteCount += numBytes;
  return result;
}

/* Print out in plain text the given body to the output stream */
void printBody(FILE *out, Body *body) {
  assert(body);
  assert(out);
  fprintf(out, "  mass = %lf\n", body->mass);
  fprintf(out, "  color = %d\n", body->color);
  fprintf(out, "  size = %d\n", body->size);
  fprintf(out, "  x= %lf\n", body->x);
  fprintf(out, "  y= %lf\n", body->y);
  fprintf(out, "  vx= %lf\n", body->vx);
  fprintf(out, "  vy= %lf\n", body->vy);
  fflush(out);
}

/* Prepare for GIF creation: open file, allocate color array */
void prepgif(char *outfilename) {
  gif = fopen(outfilename, "wb");
  assert(gif);
  colors = (int*)my_malloc(MAXCOLORS*sizeof(int));
}

/* init: reads init file and initializes variables */
void init(char* infilename, char* outfilename) {
  FILE *infile = fopen(infilename, "r");
  int i;

  assert(infile);
  fscanf(infile, "%lf", &x_min);
  printf("x_min = %lf\n", x_min);
  fscanf(infile, "%lf", &x_max);
  printf("x_max = %lf\n", x_max);
  assert(x_max > x_min);
  univ_x = x_max-x_min;
  fscanf(infile, "%lf", &y_min);
  printf("y_min = %lf\n", y_min);
  fscanf(infile, "%lf", &y_max);
  printf("y_max = %lf\n", y_max);
  assert(y_max > y_min);
  univ_y = y_max-y_min;
  fscanf(infile, "%d", &nx);
  printf("nx = %d\n", nx);
  assert(nx>=10);
  fscanf(infile, "%d", &ny);
  printf("ny = %d\n", ny);
  assert(ny>=10);
  fscanf(infile, "%lf", &K);
  printf("K = %f\n", K);
  assert(K>0);
  fscanf(infile, "%d", &nsteps);
  printf("nsteps = %d\n", nsteps);
  assert(nsteps>=1);
  fscanf(infile, "%d", &period);
  printf("period = %d\n", period);
  assert(period>0);
  fscanf(infile, "%d", &numBodies);
  printf("numBodies = %d\n", numBodies);
  assert(numBodies>0);
  bodies = (Body*)my_malloc(numBodies*sizeof(Body));
  bodies_new = (Body*)my_malloc(numBodies*sizeof(Body));
  for (i=0; i<numBodies; i++) {
    fscanf(infile, "%lf", &bodies[i].mass);
    assert(bodies[i].mass > 0);
    fscanf(infile, "%d", &bodies[i].color);
    assert(bodies[i].color >=0 && bodies[i].color<MAXCOLORS);
    fscanf(infile, "%d", &bodies[i].size);
    assert(bodies[i].size > 0);
    fscanf(infile, "%lf", &bodies[i].x);
    assert(bodies[i].x >=x_min && bodies[i].x < x_max);
    fscanf(infile, "%lf", &bodies[i].y);
    assert(bodies[i].y >=y_min && bodies[i].y < y_max);
    fscanf(infile, "%lf", &bodies[i].vx);
    fscanf(infile, "%lf", &bodies[i].vy);
#ifdef DEBUG
    printf("Body %d:\n", i);
    printBody(stdout, &bodies[i]);
#endif
  }
  for (i=0; i<numBodies; i++) {
    bodies_new[i].mass = bodies[i].mass;
    bodies_new[i].color = bodies[i].color;
    bodies_new[i].size = bodies[i].size;
  }
  fflush(stdout);
  fclose(infile);
  prepgif(outfilename);
}

/* Write time and then state of all bodies to stdout: use for debugging */
void write_plain(int time) {
  int i;

  printf("\nTime = %d\n", time);
  for (i=0; i<numBodies; i++) {
    printf("Body %d:\n", i);
    printBody(stdout, &bodies[i]);
  }
}

/* Write one frame of the GIF for given time */
void write_frame(int time) {
  int i;
  
  im = gdImageCreate(nx,ny);
  if (time == 0) {
    gdImageColorAllocate(im, 0, 0, 0);  /* black background */
    for (i=0; i<MAXCOLORS; i++)
      colors[i] = gdImageColorAllocate (im, i, 0, MAXCOLORS-i-1); 
    /* (im, i,i,i); gives gray-scale image */
    gdImageGifAnimBegin(im, gif, 1, -1);
  } else {
    gdImagePaletteCopy(im, previm);
  }
  for (i=0; i<numBodies; i++) {
    Body *body = bodies + i;
    double x = body->x;

    if (x>=1 && x<nx) {
      double y = body->y;

      if (y>=1 && y<ny) {
	int size = bodies[i].size;
	int color = bodies[i].color;
	int posx = (int)x, posy = ny-(int)y;

#ifdef DEBUG
	double r = 1.0*size/2.0;

	if (posx+r>nx-1 || posx-r<0 || posy+r>ny-1 || posy-r<0)
	  printf("Warning: printing outside of image border\n");
#endif
	gdImageFilledEllipse(im, posx, posy, size, size, colors[color]);
      }
    }
  }
  // to ensure frame is different from previous, change one pixel.
  // workaround for bug in GD
  gdImageSetPixel(im, 0, 0, (framecount++)%2);
  if (time == 0) {
    gdImageGifAnimAdd(im, gif, 0, 0, 0, 0, gdDisposalNone, NULL);
  } else {
    gdImageGifAnimAdd(im, gif, 0, 0, 0, 5, gdDisposalNone, /* previm */ NULL);
    gdImageDestroy(previm);
  }
  previm=im;
  im=NULL;
#ifdef DEBUG
  write_plain(time);
#endif
}

/* Move forward one time step.  This is the "integration step".  For
 * each body b, compute the total force acting on that body.  If you
 * divide this by the mass of b, you get b's acceleration.  So you
 * actually just calculate the b's acceleration directly, since this
 * is what you want to know.  Once you have the acceleration, proceed
 * as follows: update the position by adding the current velocity,
 * then update the velocity by adding to it the current acceleration.
 */
void update() {
  int i, j;
  Body *tmp;
#pragma omp parallel for num_threads(threadcount)
  for (i=0; i<numBodies; i++) {
    double x = bodies[i].x;
    double y = bodies[i].y;
    double vx = bodies[i].vx;
    double vy = bodies[i].vy;
    double ax = 0;
    double ay = 0;

    for (j=0; j<numBodies; j++) {
      double r, mass, dx, dy, r_squared, acceleration;

      if (j==i) continue;
      dx = bodies[j].x - x;
      dy = bodies[j].y - y;
      mass = bodies[j].mass;
      r_squared = dx*dx + dy*dy;
      if (r_squared != 0) {
	r = sqrt(r_squared);
	if (r != 0) {
	  acceleration = K*mass/(r_squared);
	  ax += acceleration*dx/r;
	  ay += acceleration*dy/r;
	}
      }
    }
    x += vx;
    y += vy;
    if (x>=x_max || x<x_min) x=x+(ceil((x_max-x)/univ_x)-1)*univ_x;
    if (y>=y_max || y<y_min) y=y+(ceil((y_max-y)/univ_y)-1)*univ_y;
    if(isnan(ax)) ax = 0;
    if(isnan(ay)) ay = 0;
    vx += ax;
    vy += ay;
    assert(!(isnan(x) || isnan(y)));
    assert(!(isnan(vx) || isnan(vy)));
    bodies_new[i].x = x;
    bodies_new[i].y = y;
    bodies_new[i].vx = vx;
    bodies_new[i].vy = vy;
  }
  tmp = bodies;
  bodies = bodies_new;
  bodies_new = tmp;
}

/* Close GIF file, free all allocated data structures */
void wrapup() {
  if (previm) gdImageDestroy(previm);
  gdImageGifAnimEnd(gif);
  fclose(gif);
  free(colors);
  free(bodies);
  free(bodies_new);
}

/* return random int in range [a,b) */
int randomInt(int a, int b) {
  double r = ((double)rand()/((double)(RAND_MAX)+(double)(1))); /* [0,1) */
  double x = a+r*(b-a); /* [a,b) */
  int result = (int)x; // rounds towards 0

  return result;
}

/* This is an alternative to the initialization by reading config
 * file.  This function instead creates random bodies within
 * certain parameters.
 * Takes 61 seconds on my Mac with period=0 (no output).
 */
void randinit(char *outfilename) {
  int i;

  x_min =-1000;
  x_max = 3000;
  y_min =-600;
  y_max = 1200;
  univ_x = x_max-x_min;
  univ_y = y_max-y_min;
  nx = 1000;
  ny = 600;
  K = .15;
  nsteps = 3000;
  // for timing, use period=0.  For testing, use a positive period.
  period = 3;
  //period = 0;
  numBodies = 1000;
  bodies = (Body*)my_malloc(numBodies*sizeof(Body));
  bodies_new = (Body*)my_malloc(numBodies*sizeof(Body));
  for (i=0; i<numBodies; i++) {
    bodies[i].mass = randomInt(2,20);
    bodies[i].color = randomInt(0,253);
    bodies[i].size = bodies[i].mass;
    bodies[i].x = randomInt(x_min, x_max);
    bodies[i].y = randomInt(y_min, y_max);
    bodies[i].vx = (1.0*randomInt(-5,5))/10.0;
    bodies[i].vy = (1.0*randomInt(-5,5))/10.0;
    assert(!(isnan(bodies[i].x) || isnan(bodies[i].y) ||
	     isnan(bodies[i].vx) || isnan(bodies[i].vy)));
  }
  for (i=0; i<numBodies; i++) {
    bodies_new[i].mass = bodies[i].mass;
    bodies_new[i].color = bodies[i].color;
    bodies_new[i].size = bodies[i].size;
  }
  prepgif(outfilename);
}

/* Perform an n-body simulation and create a GIF movie.  Usage: you
 * can either specify two arguments: the name of the configuration
 * file and the name of the GIF file you are going to create, or you
 * can specify one argument (just the name of the GIF file), in which
 * case random initialization is used. */
int main(int argc, char* argv[]) {
  int i;
  clock_t t0 = clock();
  clock_t t1;

  srand(1234567); // seed pseudo random number generator for repeatability
  if (argc == 3) {
    threadcount = atoi(argv[2]);
    randinit(argv[1]);
  } else {
    if (argc != 4) {
      printf("Usage: nbody <infilename> <outfilename>\n");
      exit(1);
    }
    threadcount = atoi(argv[3]);
    init(argv[1], argv[2]);
  }
  if (period != 0) write_frame(0);
  for (i=1; i<=nsteps; i++) {
#ifdef DEBUG
    printf("Computing time %d...", i);
    fflush(stdout);
#endif
    update();
#ifdef DEBUG
    printf("done.\n");
    fflush(stdout);
#endif
    if (period != 0 && i%period == 0) write_frame(i);
  }
  wrapup();
  t1 = clock()-t0;
  printf("Total time (seconds): %f\n", 1.0*t1/CLOCKS_PER_SEC/threadcount);
  printf("Total memory allocated (bytes): %d\n", byteCount);
  fflush(stdout); 
  return 0;
}
