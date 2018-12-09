run: pi
	srun -n 40 ./pi_mpi.out

pi: pi_mpi.c
	mpicc -std=c99 -o pi_mpi.out pi_mpi.c
run20: pi
	srun -n 20 ./pi_mpi.out
clean:
	rm -f *.out *~
