# Makefile for nbody graviational simulation

CC = cc -fopenmp -pedantic -std=c11
LIBS = -lgd -lm
FFMPEG = ffmpeg

HOST := $(shell hostname)
ifeq ($(HOST),cisc372)
RUN = srun -c 40
FLAGS =
else
# These work for OS X with GD installed by MacPorts.
# edit as needed for your system...
RUN = 
FLAGS = -I/opt/local/include -L/opt/local/lib
endif

# Converts an animated GIF out.gif to an MPEG-4 movie video.mp4
MOVOPTS = -movflags faststart -pix_fmt yuv420p -vf "scale=trunc(iw/2)*2:trunc(ih/2)*2"

# The following will take a 20 fps movie and speed it up 3x without
# dropping frames, yielding a 60 fps movie which is 1/3 as long:
# ffmpeg -i video.mp4 -r 60 -filter:v "setpts=0.333333333*PTS" output.mp4

######################### Sequential Verstion #########################

sall: s1.mp4 s2.mp4 s3.mp4 srand.mp4

clean :
	rm -f *~ *.gif *.exec *.gif *.mp4 *.mov

nbody_seq: nbody.c
	$(CC) -o nbody_seq $(FLAGS) nbody.c $(LIBS)

s1.mp4: s1.gif
	rm -f s1.mp4
	$(FFMPEG) -i s1.gif $(MOVOPTS) s1.mp4

s1.gif: nbody_seq config1.txt
	rm -f s1.gif
	$(RUN) ./nbody_seq config1.txt s1.gif
	ls

s2.mp4: s2.gif
	rm -f s2.mp4
	$(FFMPEG) -i s2.gif $(MOVOPTS) s2.mp4

s2.gif: nbody_seq config2.txt
	rm -f s2.gif
	$(RUN) ./nbody_seq config2.txt s2.gif
	ls

s3.mp4: s3.gif
	rm -f s3.mp4
	$(FFMPEG) -i s3.gif $(MOVOPTS) s3.mp4

s3.gif: nbody_seq config3.txt
	rm -f s3.gif
	$(RUN) ./nbody_seq config3.txt s3.gif
	ls

srand.mp4: srand.gif
	rm -f srand.mp4
	$(FFMPEG) -i srand.gif $(MOVOPTS) srand.mp4

srand.gif: nbody_seq
	rm -f srand.gif
	$(RUN) ./nbody_seq srand.gif
	ls


############################### OpenMP VERSION ##############################

# edit this section as needed

NTHREAD = 40

pall: p1.mp4 p2.mp4 p3.mp4 prand.mp4

nbody_omp: nbody_omp.c
	$(CC) -o nbody_omp $(FLAGS) nbody_omp.c $(LIBS)

p1.mp4: p1.gif
	rm -f p1.mp4
	$(FFMPEG) -i p1.gif $(MOVOPTS) p1.mp4

p1.gif: nbody_omp config1.txt 
	rm -f p1.gif
	$(RUN) ./nbody_omp config1.txt p1.gif $(NTHREAD)
	ls

p2.mp4: p2.gif
	rm -f p2.mp4
	$(FFMPEG) -i p2.gif $(MOVOPTS) p2.mp4

p2.gif: nbody_omp config2.txt 
	rm -f p2.gif
	$(RUN) ./nbody_omp config2.txt p2.gif $(NTHREAD)
	ls

p3.mp4: p3.gif
	rm -f p3.mp4
	$(FFMPEG) -i p3.gif $(MOVOPTS) p3.mp4

p3.gif: nbody_omp config3.txt
	rm -f p3.gif
	$(RUN) ./nbody_omp config3.txt p3.gif $(NTHREAD)
	ls

prand.mp4: prand.gif
	rm -f prand.mp4
	$(FFMPEG) -i prand.gif $(MOVOPTS) prand.mp4

prand.gif: nbody_omp
	rm -f prand.gif
	$(RUN) ./nbody_omp prand.gif $(NTHREAD)
	ls

#################################### TESTS ################################

test: test1 test2 test3

test1: s1.gif p1.gif
	diff s1.gif p1.gif

test2: s2.gif p2.gif
	diff s2.gif p2.gif

test3: s3.gif p3.gif
	diff s3.gif p3.gif
