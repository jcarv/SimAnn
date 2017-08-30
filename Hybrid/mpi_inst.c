#include "mpi_inst.h"
#include <mpi.h>

double MPI_Bcast(void *data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator) {
	double t1 = 0;
	double t2 = 0;
	int val;

	t1 = MPI_Wtime();
	val = PMPI_Bcast(data, count, datatype, 0, communicator);
	t2 = MPI_Wtime();

	return val;
}

double MPI_Send(void *data, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm communicator) {
	double t1 = 0;
	double t2 = 0;
	int val;

	t1 = MPI_Wtime();
	val = PMPI_Send(data, count, datatype, dest, tag, communicator);
	t2 = MPI_Wtime();

	return val;
}

double MPI_Recv(void *data, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm communicator, MPI_Status *status) {
	double t1 = 0;
	double t2 = 0;
	int val;

	t1 = MPI_Wtime();
	val = PMPI_Recv(data, count, datatype, source, tag, communicator, status);
	t2 = MPI_Wtime();

	return val;
}
