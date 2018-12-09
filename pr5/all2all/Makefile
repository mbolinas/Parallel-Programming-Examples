test: all2all driver
	mpicc -std=c99 -g all2all.o driver.o -o driver.exe
	srun -n 5 ./driver.exe

all2all: all2all.c
	mpicc -std=c99 -g -c all2all.c	

driver: driver.c
	mpicc -std=c99 -g -c driver.c

clean:
	rm -f *.o *.exe *# *~ *.out
