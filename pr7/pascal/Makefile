CC = cc -fopenmp -pedantic -std=c11
RUN = srun -c 40

all: run

pascal: pascal.c
	$(CC) -o pascal pascal.c

pascal_omp: pascal_omp.c
	$(CC) -o pascal_omp pascal_omp.c

run_omp: pascal_omp
	$(RUN) ./pascal_omp > out_omp.txt

run_seq: pascal
	$(RUN) ./pascal > out_seq.txt

run: run_seq run_omp
	diff out_omp.txt out_seq.txt

clean:
	rm -f *~ *.txt pascal pascal_omp
