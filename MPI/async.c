#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int world_size, world_rank, left, right;
    int buffer, buffer2, p = 0;

    MPI_Request request2;
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	MPI_Request request[world_size];
	int flag[world_size];
	p = 0;
	while(p < world_size) {
	flag[p] = 0;
		p++;
	}

	p = 0;
	while(p < world_size) {
		MPI_Irecv(&buffer2, 1, MPI_INT, p, 123, MPI_COMM_WORLD, &request[p]);
		p++;
	}
	sleep(5);

	buffer = 156;
    if(world_rank == 2) {
		p = 0;
		while(p < world_size) {
		    MPI_Send(&buffer, 1, MPI_INT, p, 123, MPI_COMM_WORLD);
			p++;
		}
	}


	//sleep(1);
	p = 0;
	while(p < world_size) {
		MPI_Test(&request[p], &flag[p], &status);
		p++;
	}

	p = 0;
	while(p < world_size) {
		if(flag[p] != 0) {
			printf("%dreceived from %d, message:%d\n", world_rank, 2, buffer2);
		}
		p++;
	}

    //MPI_Wait(&request, &status);
    //MPI_Wait(&request2, &status);


    MPI_Finalize();
    return 0;
}
