
seq: matrix.c
	gcc -std=c11 -o seq.out matrix.c -lm

cuda: matrix.cu
	nvcc -o cuda.out matrix.cu

testseq: seq
	./seq.out 3 3

bigtestseq: seq
	./seq.out 10000 10000

testcuda: cuda
	srun -n 1 --gres=gpu:1 cuda.out 3 3

bigtestcuda: cuda
	srun -n 1 --gres=gpu:1 cuda.out 10000 10000

clean:
	rm -f *.out *~ *#
