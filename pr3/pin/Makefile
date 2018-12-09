test1: pin_mpi
	srun -n 40 ./pin_mpi.out 10000000000

pin_mpi: pin_mpi.c
	mpicc -std=c99 -o pin_mpi.out pin_mpi.c -lm

clean:
	rm -f *~ *.out *#
