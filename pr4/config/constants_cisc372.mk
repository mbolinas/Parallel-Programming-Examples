# DO NOT MODIFY THIS
RUN = srun -n 1
MPIRUN = srun
OMPRUN = srun -c $(NCORES)
CUDA_RUN = srun -n 1 --gres=gpu:1 

FLAGS =
FLAGS_DEBUG = $(FLAGS) -DDEBUG
