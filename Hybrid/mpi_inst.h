#include <mpi.h>

int MPI_Bcast(void *data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator);
int MPI_Send(void *data, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm communicator);
int MPI_Recv(void *data, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm communicator, MPI_Status *status);