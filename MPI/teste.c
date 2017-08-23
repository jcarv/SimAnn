#include <mpi.h>
#include <stdio.h>

int main(int argc,char *argv[]){
    int rank,msg;
    MPI_Status status;



    

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    //processo 0 envia e o processo 1 recebe
    if(rank == 0){
	int m[2][2];
	m[0][0]=1;
	m[0][1]=2;
	m[1][0]=3;
	m[1][1]=4;
        MPI_Send(&m,4,MPI_INT,1,0,MPI_COMM_WORLD);
    }
    else if(rank == 1){
    int m2[2][2];
        MPI_Recv(&m2,4,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        printf("Recebido a mensagem: %d\n",m2[1][0]);
    }
    MPI_Finalize();
    return 0;
}

