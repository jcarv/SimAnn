#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include "mpi_inst.h"
#include "jobshop.h"
#include "simann.h"

int JOBS, MACHINES, SWAPS, FROZEN;
double T_IDLE;
unsigned int seed;

void master(int exchange_parameter, int final_exchange) {
	char *linha,*token;
	MPI_Status status;

	//Get the number of processes;
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	//Get the rank of the process
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	linha = (char*) malloc(sizeof(char)*20);

    //le a primeira linha do ficheiro
    char* sink = fgets(linha, 20, stdin);
	
	//le numero de jobs
    token = strtok(linha, " ");
    JOBS = atoi(token);
    //le numero de machines
    token = strtok(NULL, " ");
    MACHINES = atoi(token);

	int **configuration;
	configuration = alloc_2d_int();

	load_configuration(configuration);
	
	//envia dimensoes e configuracao
	MPI_Bcast(&JOBS, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&MACHINES, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&configuration[0][0], JOBS*MACHINES, MPI_INT, 0, MPI_COMM_WORLD);

	int** initial_solution;
	initial_solution = alloc_2d_int();
	generate_solution(initial_solution, configuration);

	int **best_guess;
	best_guess = alloc_2d_int();
	
	//master_shared_simulated_annealing(best_guess, configuration, final_exchange);
	shared_simulated_annealing(best_guess, exchange_parameter, final_exchange, initial_solution, configuration);
	
	MPI_Bcast(&best_guess[0][0], JOBS*MACHINES, MPI_INT, 0, MPI_COMM_WORLD);

	simulated_annealing(best_guess, configuration);
	
	int process_cost[world_size];
	int best_cost;
	best_cost = cost(best_guess);
	int p = 1;
	while(p < world_size) {
	
		MPI_Recv(&process_cost[p], 1, MPI_INT, p, 65, MPI_COMM_WORLD, &status);
		if(process_cost[p] < best_cost) {
			best_cost = process_cost[p];
		}
		//printf("Process: %d, Cost: %d\n", p, process_cost[p]);
		p++;
	}

//	MPI_IRecv(&process_cost, 1, MPI_INT, 1, 65, MPI_COMM_WORLD, &status);
	//printf("Process: %d, Cost: %d\n",world_rank,cost(best_guess));
	printf("best cost: %d\n", best_cost);
	my_free(configuration);
	my_free(best_guess);
}

void slave(int exchange_parameter, int final_exchange) {
	double t1, t2; 
	double t_Comp = 0;
	//Get the number of processes;
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	//Get the rank of the process
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	//inicialização diferente para cada processo
	seed = time(NULL);
	seed += world_rank*3;

	//recebe dimensoes e configuracao
	MPI_Status status;
	MPI_Bcast(&JOBS, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&MACHINES, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int** configuration;
	configuration = alloc_2d_int();
	MPI_Bcast(&configuration[0][0], JOBS*MACHINES, MPI_INT, 0, MPI_COMM_WORLD);
	
	int** initial_solution;
	initial_solution = alloc_2d_int();
	generate_solution(initial_solution, configuration);
	int** dummy;
	shared_simulated_annealing(dummy, exchange_parameter, final_exchange, initial_solution, configuration);

	my_free(initial_solution);
	initial_solution = alloc_2d_int();
	MPI_Bcast(&initial_solution[0][0], JOBS*MACHINES, MPI_INT, 0, MPI_COMM_WORLD);

	simulated_annealing(initial_solution, configuration);

	int cost_solution;
	cost_solution = cost(initial_solution);
	//printf("%d process, cost %d\n", world_rank, cost_solution);
	MPI_Send(&cost_solution, 1, MPI_INT, 0, 65, MPI_COMM_WORLD);

	my_free(configuration);
	my_free(initial_solution);
}

int main(int argc, char* argv[]){
	SWAPS = 2;
	FROZEN = 5;
	T_COMP,T_COMM,T_IDLE = 0;
	double t1, t2; 
	//exchange_parameter = 1 : Comunicação continua
	int exchange_parameter = 1, final_exchange = 8;

	t1 = MPI_Wtime();

	//Initialize the MPI environment
	MPI_Init(NULL, NULL);

	//Get the rank of the process
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	
	if (world_rank == 0) {
		master(exchange_parameter, final_exchange);
	} else if (world_rank != 0) {
		slave(exchange_parameter, final_exchange);
	}

	//Finalize the MPI environment.
	t2 = MPI_Wtime();
	printf("Process %d Computation time is %f\n", world_rank, T_COMP);
	printf("Process %d Communication time is %f\n", world_rank, T_COMM);
	printf("Process %d Idle time is %f\n", world_rank, T_IDLE);
	printf("Process %d Elapsed time is %f\n", world_rank, t2 - t1);
	MPI_Finalize();

	return 0;
}
