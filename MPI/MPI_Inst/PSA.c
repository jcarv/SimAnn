#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define SWAPS 2
#define FROZEN 5

int JOBS, MACHINES, WORKERS, CONVERGED;

unsigned int seed;
//para evitar que a seed seja uma variavel partilhada(foram feitos testes, e caso seja partilhada a execução é consideravelmente mais lenta)
#pragma omp threadprivate(seed)

void load_configuration(int configuration[JOBS][MACHINES]) {
    char *linha,*token;
    linha = (char*) malloc(sizeof(char)*20);
    int job,machine,tempo;

    while(fgets(linha, 20, stdin)){        
        
        token = strtok(linha, " ");
        job = atoi(token);

        token = strtok(NULL, " ");
        machine = atoi(token);

        token = strtok(NULL, " ");
        tempo = atoi(token);
        
        //preenche a matriz m com o tempo no determinado job e maquina
        configuration[job-1][machine-1] = tempo;
    }
}

void copy_matrix(int source[JOBS][MACHINES], int destination[JOBS][MACHINES]) {
    int i = 0, j = 0;
    
    while(i < JOBS) {
        j = 0;
        while(j < MACHINES) {
            destination[i][j] = source[i][j];
            j++;
        }
        i++;
    }
}

void generate_solution(int solution[JOBS][MACHINES], int configuration[JOBS][MACHINES]) {

    copy_matrix(configuration, solution);

    for(int i = 0; i < JOBS; i++){
        int machine = rand_r(&seed) % MACHINES;
        //solution[i][machine] = configuration[i][machine];

        for(int k = 0; k < MACHINES; k++){
            if(machine != k){
                solution[i][k] = 0;
            }
        }
    }
}

void generate_neighbour(int neighbour[JOBS][MACHINES], int solution[JOBS][MACHINES], int configuration[JOBS][MACHINES]) {
    int original_machine = 0;
    int job_swap;
    
    copy_matrix(solution, neighbour);
    
    for(int i = 0; i < SWAPS; i++){
        job_swap = rand_r(&seed) % JOBS;
        for(int j = 0; j < MACHINES; j++){
            if(solution[job_swap][j] != 0){
                original_machine = j;
                break;
            }
        }

        int destination_machine = original_machine;
        
        while(destination_machine == original_machine){
            destination_machine = rand_r(&seed) % MACHINES;
        }

        neighbour[job_swap][original_machine] = 0;
        neighbour[job_swap][destination_machine] = configuration[job_swap][destination_machine];
    }
}

int cost(int solution[JOBS][MACHINES]) {
    int res = 0;

    for(int i = 0; i < JOBS; i++){
        for(int j = 0; j < MACHINES; j++){
            res += solution[i][j];
        }
    }
    
    return res;
}


void output_configuration(int configuration[JOBS][MACHINES]) {
    int i = 0, j = 0;
    
    printf("Configuration:\n");
    
    while(i < JOBS) {
        j = 0;
        
        while(j < MACHINES) {
            printf(" %d ", configuration[i][j]);
            
            j++;
        }
        printf("\n");
        i++;
    }
    printf("\n");
}


double randZeroToOne()
{
    return rand_r(&seed) / (RAND_MAX + 1.);
}

double probability(double delta, double temperature) {
    double res = exp(-delta/temperature);
    
    if (res < 1) {
        return res;
    } else {
        return 1;
    }
}

int find_best(int best_guesses[WORKERS][JOBS][MACHINES]) {
	int best_cost, worker_id;

	worker_id = 0;
	best_cost = cost(best_guesses[worker_id]);
	
	for(int w = 0; w < WORKERS; w++){
		int current_cost = cost(best_guesses[w]);
		if(current_cost < best_cost) {
			worker_id = w;
			best_cost = current_cost;
		}
	}

	return worker_id;
}

void shared_simulated_annealing(int exchange_parameter, int final_exchange, int current_solution[JOBS][MACHINES], int configuration[JOBS][MACHINES], int best_guesses[WORKERS][JOBS][MACHINES]){
	int iteration = 0, exchanges = 0;
	double initial_temperature = 2000;
    int t_temperature = 15000;     
    double cooling_rate = 0.8;

    for(; initial_temperature > 0.00001; initial_temperature*= cooling_rate){
        for(int counter = 0; counter < (t_temperature/WORKERS); counter++){
            int neighbour_solution[JOBS][MACHINES];
	        generate_neighbour(neighbour_solution, current_solution, configuration);

    	    double current_cost = cost(best_guesses[omp_get_thread_num()]);
	        double neighbour_cost = cost(neighbour_solution);

    	    double delta = current_cost - neighbour_cost;
    	    double p = probability(delta, initial_temperature);

    	    if(delta > 0) {
    		    copy_matrix(neighbour_solution, current_solution);
				copy_matrix(neighbour_solution, best_guesses[omp_get_thread_num()]);
			} else if(randZeroToOne()>p) {
				copy_matrix(neighbour_solution, current_solution);
				copy_matrix(neighbour_solution, best_guesses[omp_get_thread_num()]);
			}
		}

		if(iteration == exchange_parameter) {
			iteration = 0;

			#pragma omp barrier
			//#pragma omp single
			//{
			//	printf("\n\n--- Partial search costs ---\n\n");
			//	for(int w = 0; w < WORKERS; w++){
			//		printf("Worker %d, partial search cost: %d\n", w, cost(best_guesses[w]));
			//	}
			//}

			int best_worker_id = find_best(best_guesses);

			for(int w = 0; w < WORKERS; w++){
				if(w != best_worker_id) {
					copy_matrix(best_guesses[best_worker_id] ,best_guesses[w]);
				}
			}
		
			copy_matrix(best_guesses[best_worker_id], current_solution);
			exchanges++;
			#pragma omp barrier
		} else {
			iteration++;
		}

		if(exchanges == final_exchange) {
			break;
		}	
    }
	
}

void simulated_annealing(int current_solution[JOBS][MACHINES], int configuration[JOBS][MACHINES], int best_guesses[WORKERS][JOBS][MACHINES]){
	double initial_temperature = 2000;
    int t_temperature = 15000;     
    double cooling_rate = 0.8;
	int static_solution = 0;
	int cost_solution = cost(current_solution);

    for(; initial_temperature > 0.00001; initial_temperature*= cooling_rate){
        for(int counter = 0; counter < (t_temperature/WORKERS); counter++){
            int neighbour_solution[JOBS][MACHINES];
	        generate_neighbour(neighbour_solution, current_solution, configuration);

    	    double current_cost = cost(best_guesses[omp_get_thread_num()]);
	        double neighbour_cost = cost(neighbour_solution);

    	    double delta = current_cost - neighbour_cost;
    	    double p = probability(delta, initial_temperature);

    	    if(delta > 0) {
    		    copy_matrix(neighbour_solution, current_solution);
				copy_matrix(neighbour_solution, best_guesses[omp_get_thread_num()]);
			} else if(randZeroToOne()>p) {
				copy_matrix(neighbour_solution, current_solution);
				copy_matrix(neighbour_solution, best_guesses[omp_get_thread_num()]);
			}

			if(CONVERGED != 0) {
				break;
			}
	    }
	
		if(cost_solution <= cost(current_solution)) {
			static_solution++;
		} else {
			static_solution = 0;
			cost_solution = cost(current_solution);
		}
	
		if(static_solution == FROZEN || CONVERGED != 0) {
			CONVERGED = 1;
			//printf("STATIC_SOLUTION!\n");
			break;
		}
	}
}

void worker_node(int exchange_parameter, int final_exchange, int initial_solution[WORKERS][JOBS][MACHINES], int best_guesses[WORKERS][JOBS][MACHINES], int configuration[JOBS][MACHINES]) {
	int current_solution[JOBS][MACHINES];
	copy_matrix(initial_solution[omp_get_thread_num()], current_solution);

	shared_simulated_annealing(exchange_parameter,final_exchange, current_solution, configuration, best_guesses);

	simulated_annealing(best_guesses[0], configuration, best_guesses);
}


int main(int argc, char* argv[]){
	WORKERS = omp_get_max_threads();
	CONVERGED = 0;
	char *linha,*token;
	int exchange_parameter = 1;
	int final_exchange = 8;
    FILE *fp = fopen(argv[1], "a");

	seed = time(NULL);

	linha = (char*) malloc(sizeof(char)*20);
    //le a primeira linha do ficheiro
    char* sink = fgets(linha, 20, stdin);

    //le numero de jobs
    token = strtok(linha, " ");
    JOBS = atoi(token);
    //le numero de machines
    token = strtok(NULL, " ");
    MACHINES = atoi(token);

    int configuration[JOBS][MACHINES];
    load_configuration(configuration);

	//generate_solutions
	int current_solutions[WORKERS][JOBS][MACHINES];
	int best_guesses[WORKERS][JOBS][MACHINES];
	double timer = omp_get_wtime();
	#pragma omp parallel for 
	for(int w = 0; w < WORKERS; w++){
		seed += omp_get_thread_num();
		generate_solution(current_solutions[w], configuration);
		copy_matrix(current_solutions[w], best_guesses[w]);
	}

	//printf("Workers: %d\n", WORKERS);
	//printf("\n\n--- Initial costs ---\n\n");
	//for(int w = 0; w < WORKERS; w++){
	//	printf("Worker %d, initial cost: %d\n", w, cost(current_solutions[w]));
	//}

	#pragma omp parallel for num_threads(WORKERS) firstprivate(WORKERS,exchange_parameter, final_exchange, configuration,current_solutions) shared(best_guesses)
	for(int w = 0; w < WORKERS; w++){
		seed += omp_get_thread_num();
		worker_node(exchange_parameter, final_exchange, current_solutions, best_guesses, configuration);
		//printf("tid %d worked %d\n",omp_get_thread_num(), w);
	}

	//printf("\n\n--- Final Costs ---\n\n");
	//for(int w = 0; w < WORKERS; w++){
	//	printf("Worker %d, final cost: %d\n", w, cost(best_guesses[w]));
	//}

	//printf("\n\n--- Best Cost ---\n\n");
    fprintf(fp, "%f\n", omp_get_wtime() - timer);
	printf("tempo %f\n", omp_get_wtime() - timer);
	int id = find_best(best_guesses);
	//printf("Worker %d, cost: %d\n\n", id, cost(best_guesses[id]));
	//fprintf(fp, "Jobs: %d Machines: %d\n", JOBS, MACHINES);
	printf("custo %d\n", cost(best_guesses[id]));
	fclose(fp);

//	output_configuration(best_guesses[id]);
//	printf("Duration:%f\n", omp_get_wtime() - timer);
    return 0;
}

