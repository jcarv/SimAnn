MPI_COMPILE_FLAGS = $(shell mpicc --showme:compile)
MPI_LINK_FLAGS = $(shell mpicc --showme:link)

all: mpi_inst.o jobshop.o simann.o MPIPSA

mpi_inst.o : mpi_inst.c
	$(CC) $(MPI_COMPILE_FLAGS) -c mpi_inst.c $(MPI_LINK_FLAGS)

jobshop.o : jobshop.c
	$(CC) $(MPI_COMPILE_FLAGS) -c jobshop.c $(MPI_LINK_FLAGS) -lm
	
simann.o : simann.c
	$(CC) $(MPI_COMPILE_FLAGS) -c simann.c $(MPI_LINK_FLAGS)

MPIPSA : jobshop.o simann.o
	$(CC) $(MPI_COMPILE_FLAGS) -o MPIPSA MPIPSA.c mpi_inst.o $(MPI_LINK_FLAGS) jobshop.o simann.o -lm

clean :
	rm -f *.o
	rm -f MPIPSA

