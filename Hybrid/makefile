
all: jobshop.o simann.o MPMPIPSA

jobshop.o : jobshop.c
	gcc -c jobshop.c -lm

simann.o : simann.c
	mpicc -c simann.c

MPMPIPSA : jobshop.o simann.o
	mpicc -fopenmp -std=c99 -o MPMPIPSA MPMPIPSA.c jobshop.o simann.o -lm

clean :
	rm -f *.o
	rm -f MPMPIPSA

