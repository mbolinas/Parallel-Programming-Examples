# DO NOT MODIFY THIS
RUN = srun -n 1
MPIRUN = srun
OMPRUN = srun -c 40
CUDA_RUN = srun -n 1 --gres=gpu:1 
CC = cc -pedantic -std=c11 -DM_PI=3.14159265358979323846265
MPICC = mpicc -pedantic -std=c11 -DM_PI=3.14159265358979323846265

FLAGS =
FLAGS_DEBUG = $(FLAGS) -DDEBUG
