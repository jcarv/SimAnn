#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	int rank, msg;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	/*Process 0 sends and Process 1 receives*/
	if (rank == 0) {
		msg = 123456;
		MPI_Send(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
	} else if (rank == 1) {
		MPI_Recv(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		printf("Received %d\n", msg);
	}

	MPI_Finalize();
	return 0;
}
