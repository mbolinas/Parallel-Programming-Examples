
nb: integral_nb.exe
	srun -n 5 ./integral_nb.exe

integral_nb.exe: integral_nb.c
	-mpicc -std=c99 -o integral_nb.exe integral_nb.c -lm

clean:
	rm -f *.exe *.out *~ *#
