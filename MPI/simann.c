#include <stdio.h>
#include "simann.h"
#include "jobshop.h"
#include "mpi_inst.h"

void simulated_annealing(int** current_solution, int** configuration){
    int t_temperature = 15000, static_solution = 0, cost_solution = cost(current_solution), converged = 0;
	double initial_temperature = 2000, cooling_rate = 0.8;
	double comm_time = 0;
	double t1 = 0;
	double t2 = 0;
	MPI_Status status;

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	MPI_Request request[world_size];
	int flag[world_size];

	int p = 0;
	while(p < world_size) {
		flag[p] = 0;
		p++;
	}

	p = 0;
	while(p < world_size) {
		if(p != world_rank) {
			MPI_Irecv(&converged, 1, MPI_INT, p, 123, MPI_COMM_WORLD, &request[p]);
		}
		p++;
	}

    for(; initial_temperature > 0.00001; initial_temperature*= cooling_rate){
        for(int counter = 0; counter < (t_temperature/world_size); counter++){
            int** neighbour_solution;
			neighbour_solution = alloc_2d_int();
	        generate_neighbour(neighbour_solution, current_solution, configuration);

    	    double current_cost = cost(current_solution);
	        double neighbour_cost = cost(neighbour_solution);

    	    double delta = current_cost - neighbour_cost;
    	    double p = probability(delta, initial_temperature);

    	    if(delta > 0) {
    		    copy_matrix(neighbour_solution, current_solution);
			} else if(randZeroToOne()>p) {
				copy_matrix(neighbour_solution, current_solution);
			}

			my_free(neighbour_solution);
	    }
	
		if(cost_solution <= cost(current_solution)) {
			static_solution++;
		} else {
			static_solution = 0;
			cost_solution = cost(current_solution);
		}

		p = 0;
		while(p < world_size) {
			if(p != world_rank) {
				t1 = MPI_Wtime();
				MPI_Test(&request[p],&flag[p],&status);
				t2 = MPI_Wtime();
				printf("\nProcess %d test comm time:%f\n",world_rank, t2-t1);
			}
			p++;
		}

		if(static_solution == FROZEN || converged != 0) {
			converged = 1;
			p = 0;
			while(p < world_size) {
				if(p != world_rank) {
					comm_time += MPI_Send(&converged, 1, MPI_INT, p, 123, MPI_COMM_WORLD);
				}
				p++;
			}
			printf("\nProcess %d independent sim ann comm time:%f\n",world_rank, comm_time);
			break;
		}
	}
}

void shared_simulated_annealing(int **best_guess, int exchange_parameter, int final_exchange, int** current_solution, int** configuration){
	MPI_Status status;
	int iteration = 0, exchanges = 0, t_temperature = 15000, best_cost = 0, p = 1;
	double initial_temperature = 2000, cooling_rate = 0.8;
	double comm_time = 0;

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    for(; initial_temperature > 0.00001; initial_temperature*= cooling_rate){
        for(int counter = 0; counter < (t_temperature/world_size-1); counter++){
            int** neighbour_solution;
			neighbour_solution = alloc_2d_int();
	        generate_neighbour(neighbour_solution, current_solution, configuration);

			double current_cost = cost(current_solution);
	        double neighbour_cost = cost(neighbour_solution);

    	    double delta = current_cost - neighbour_cost;
    	    double p = probability(delta, initial_temperature);

    	    if(delta > 0) {
    		    copy_matrix(neighbour_solution, current_solution);
			} else if(randZeroToOne()>p) {
				copy_matrix(neighbour_solution, current_solution);
			}

			my_free(neighbour_solution);
		}

		if(iteration == exchange_parameter) {
			iteration = 0;

			if(world_rank != 0) {
				comm_time += MPI_Send(&current_solution[0][0], JOBS*MACHINES, MPI_INT, 0, 0, MPI_COMM_WORLD);

				if(exchanges == final_exchange-1) {
					break;
				} else {
					comm_time += MPI_Bcast(&current_solution[0][0], JOBS*MACHINES, MPI_INT, 0, MPI_COMM_WORLD);
					//printf("process %d, post_comm_cost:%d\n", world_rank, cost(current_solution));
				}		
			} else {
				p = 1;
				best_cost = cost(current_solution);
				copy_matrix(current_solution, best_guess);
				while(p < world_size) {
					if(p != world_rank) {
						int **guess;
						guess = alloc_2d_int();

						comm_time += MPI_Recv(&guess[0][0], JOBS*MACHINES, MPI_INT, p, 0, MPI_COMM_WORLD, &status);
						int guess_cost = cost(guess);

						if(guess_cost <= best_cost) {
							copy_matrix(guess, best_guess);
							best_cost = guess_cost;
						}

						my_free(guess);
					}
					p++;
				}
		
				if(exchanges == final_exchange-1) {
					printf("\nProcess %d shared sim ann comm time:%f\n",world_rank, comm_time);
					break;
				} else {
					comm_time += MPI_Bcast(&best_guess[0][0], JOBS*MACHINES, MPI_INT, 0, MPI_COMM_WORLD);
					copy_matrix(best_guess, current_solution);
					//printf("process %d, post_comm_cost:%d\n", world_rank, cost(current_solution));
				}
			}

			exchanges++;
		} else {
			iteration++;
		}
    }
}
