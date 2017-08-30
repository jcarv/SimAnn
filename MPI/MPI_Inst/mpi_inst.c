#include <mpi.h>
#include <stdio.h>
#include "mpi_inst.h"

double T_COMM;

int MPI_Bcast(void *buf, int count, MPI_Datatype datatype, int root, MPI_Comm communicator) {
	double t1 = 0;
	double t2 = 0;
	int val;

	t1 = MPI_Wtime();
	val = PMPI_Bcast(buf, count, datatype, 0, communicator);
	t2 = MPI_Wtime();

	T_COMM += t2-t1;

	return val;
}

int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm communicator) {
	double t1 = 0;
	double t2 = 0;
	int val;

	t1 = MPI_Wtime();
	val = PMPI_Send(buf, count, datatype, dest, tag, communicator);
	t2 = MPI_Wtime();

	T_COMM += t2-t1;
	return val;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm communicator, MPI_Status *status) {
	double t1 = 0;
	double t2 = 0;
	int val;

	t1 = MPI_Wtime();
	val = PMPI_Recv(buf, count, datatype, source, tag, communicator, status);
	t2 = MPI_Wtime();

	T_COMM += t2-t1;

	return val;
}