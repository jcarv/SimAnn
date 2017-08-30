
all: jobshop.o simann.o MPIPSA

jobshop.o : jobshop.c
	gcc -c jobshop.c -lm
	
simann.o : simann.c
	mpicc -c simann.c

MPIPSA : jobshop.o simann.o
	mpicc -o MPIPSA MPIPSA.c jobshop.o simann.o -lm

clean :
	rm -f *.o
	rm -f MPIPSA

