run: pingpong
	srun -n 40 ./pingpong.out
pingpong: pingpong.c
	mpicc -std=c99 -o pingpong.out pingpong.c
clean:
	rm -f *~ *.out *#
