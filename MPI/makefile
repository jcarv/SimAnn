
all: mpi_inst jobshop.o simann.o MPIPSA

jobshop.o : jobshop.c
	gcc -c jobshop.c -lm

mpi_inst.o : mpi_inst.c
	mpicc -c mpi_inst.c

simann.o : simann.c
	mpicc -c simann.c

MPIPSA : jobshop.o mpi_inst.o simann.o
	mpicc -o MPIPSA MPIPSA.c jobshop.o mpi_inst.o simann.o -lm

clean :
	rm -f *.o
	rm -f MPIPSA

