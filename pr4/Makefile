include ./config/constants.mk

#         k          nx     nsteps   wstep    filename
CONFIG1 = 0.30      200     15000    150
CONFIG2 = 0.30     2000    150000    150 
CONFIG3 = 0.30  1000000     15000      0 
CONFIG4 = 0.40       10         5      1 
CONFIG5 = 0.10        2         5      2 
CONFIG6 = 0.20        5         1      1 
CONFIG7 = 0.30       10         5      3

all: timings

test: heat1_seq.gif heat2_seq.gif heat3_seq.gif heat4_seq.gif heat5_seq.gif \
     heat6_seq.gif heat7_seq.gif \
     heat1_mpi.gif heat2_mpi.gif heat3_mpi.gif heat4_mpi.gif heat5_mpi.gif \
     heat6_mpi.gif heat7_mpi.gif \
     Makefile
	diff heat1_seq.gif heat1_mpi.gif
	diff heat2_seq.gif heat2_mpi.gif
	diff heat3_seq.gif heat3_mpi.gif
	diff heat4_seq.gif heat4_mpi.gif
	diff heat5_seq.gif heat5_mpi.gif
	diff heat6_seq.gif heat6_mpi.gif
	diff heat7_seq.gif heat7_mpi.gif

parout: 
	mkdir parout

seqout:
	mkdir seqout

heat_debug: heat.c seqout
	$(CC) $(FLAGS) -DDEBUG -o heat heat.c $(LIB_GD)

heat: heat.c
	$(CC) $(FLAGS) -o heat heat.c $(LIB_GD)

heat1_seq.gif: heat
	$(RUN) ./heat $(CONFIG1)     heat1_seq.gif

heat2_seq.gif: heat
	$(RUN) ./heat $(CONFIG2)     heat2_seq.gif

heat3_seq.gif: heat
	$(RUN) ./heat $(CONFIG3)     heat3_seq.gif

heat4_seq.gif: heat
	$(RUN) ./heat $(CONFIG4)     heat4_seq.gif

heat5_seq.gif: heat
	$(RUN) ./heat $(CONFIG5)     heat5_seq.gif

heat6_seq.gif: heat
	$(RUN) ./heat $(CONFIG6)     heat6_seq.gif

heat7_seq.gif: heat
	$(RUN) ./heat $(CONFIG7)     heat7_seq.gif


# Converts an animated GIF out.gif to an MPEG-4 movie video.mp4
MOVIE_FLAGS = -movflags faststart -pix_fmt yuv420p -vf "scale=trunc(iw/2)*2:trunc(ih/2)*2"

heat1_seq.mp4: heat1_seq.gif
	$(FFMPEG) -i heat1_seq.gif $(MOVIE_FLAGS) heat1_seq.mp4

heat2_seq.mp4: heat2_seq.gif
	$(FFMPEG) -i heat2_seq.gif $(MOVIE_FLAGS) heat2_seq.mp4


# MPI Version...

heat_mpi_debug: heat_mpi.c Makefile parout
	$(MPICC) $(FLAGS) -DDEBUG -o heat_mpi heat_mpi.c $(LIB_GD)

heat_mpi: heat_mpi.c Makefile
	$(MPICC) $(FLAGS) -o heat_mpi heat_mpi.c $(LIB_GD)

heat1_mpi.gif: heat_mpi
	$(MPIRUN) -n 4 ./heat_mpi $(CONFIG1) heat1_mpi.gif

heat2_mpi.gif: heat_mpi
	$(MPIRUN) -n 1 ./heat_mpi $(CONFIG2) heat2_mpi.gif

heat3_mpi.gif: heat_mpi
	$(MPIRUN) -n 4 ./heat_mpi $(CONFIG3) heat3_mpi.gif

heat4_mpi.gif: heat_mpi
	$(MPIRUN) -n 2 ./heat_mpi $(CONFIG4) heat4_mpi.gif

heat5_mpi.gif: heat_mpi
	$(MPIRUN) -n 3 ./heat_mpi $(CONFIG5) heat5_mpi.gif

heat6_mpi.gif: heat_mpi
	$(MPIRUN) -n 2 ./heat_mpi $(CONFIG6) heat6_mpi.gif

heat7_mpi.gif: heat_mpi
	$(MPIRUN) -n 4 ./heat_mpi $(CONFIG7) heat7_mpi.gif


timings: heat_mpi
	$(MPIRUN) -n 1 ./heat_mpi $(CONFIG3) tmp.gif
	$(MPIRUN) -n 2 ./heat_mpi $(CONFIG3) tmp.gif
	$(MPIRUN) -n 3 ./heat_mpi $(CONFIG3) tmp.gif
	$(MPIRUN) -n 4 ./heat_mpi $(CONFIG3) tmp.gif
	$(MPIRUN) -n 5 ./heat_mpi $(CONFIG3) tmp.gif
	$(MPIRUN) -n 6 ./heat_mpi $(CONFIG3) tmp.gif
	$(MPIRUN) -n 7 ./heat_mpi $(CONFIG3) tmp.gif
	$(MPIRUN) -n 8 ./heat_mpi $(CONFIG3) tmp.gif
	$(MPIRUN) -n 16 ./heat_mpi $(CONFIG3) tmp.gif
	$(MPIRUN) -n 24 ./heat_mpi $(CONFIG3) tmp.gif
	$(MPIRUN) -n 32 ./heat_mpi $(CONFIG3) tmp.gif
	$(MPIRUN) -n 40 ./heat_mpi $(CONFIG3) tmp.gif

clean:
	rm -f *.gif heat heat_mpi *.mp4 seqout/* parout/*
