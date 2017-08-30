#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <omp.h>

#define SWAPS 2
#define STUCK 20

int JOBS, MACHINES;
unsigned int seed;

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

int cost(int solution[JOBS][MACHINES]) {
    int res = 0;

    for(int i = 0; i < JOBS; i++){
        for(int j = 0; j < MACHINES; j++){
            res += solution[i][j];
        }
    }
    
    return res;
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


int simulated_anneling(double initial_temperature, double cooling_rate, int t_temperature, int configuration[JOBS][MACHINES]){
    int initial_solution[JOBS][MACHINES];
	int static_solution = 0;

    generate_solution(initial_solution, configuration);
    
    int current_solution[JOBS][MACHINES];
    copy_matrix(initial_solution, current_solution);
    
	for(; initial_temperature > 0.00001; initial_temperature*= cooling_rate){
		int cost_solution = cost(current_solution);

        for(int counter = 0; counter < t_temperature; counter++){
            int neighbour_solution[JOBS][MACHINES];
            generate_neighbour(neighbour_solution, current_solution, configuration);
                
            double current_cost = cost(current_solution);
            double neighbour_cost = cost(neighbour_solution);
                
            double delta = current_cost - neighbour_cost;
            double p = probability(delta, initial_temperature);

            if(delta > 0) {
                copy_matrix(neighbour_solution, current_solution);
            } 
            else if(randZeroToOne()>p) {
                copy_matrix(neighbour_solution, current_solution);
            }
        }

		if(cost_solution <= cost(current_solution)) {
			static_solution++;
		} else {
			static_solution = 0;
		}

		if(static_solution == STUCK) {
			//printf("STATIC_SOLUTION!\n");
			break;
		}
    }
    
    //printf("Initial solution:\n");
    //output_configuration(initial_solution);
    //printf("Initial cost: %d\n", cost(initial_solution));
    //printf("\nBest solution found:\n");
   	//output_configuration(current_solution);
    //printf("Cost: %d\n", cost(current_solution));

	return cost(current_solution);
}


int main(int argc, char* argv[]){
	seed = time(NULL);
    double initial_temperature = 2000;
    int t_temperature = 15000;     
    double cooling_rate = 0.7;
    FILE *fp = fopen(argv[1], "a");
    char *linha,*token;
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

    double time = omp_get_wtime();
    int cost = simulated_anneling(initial_temperature, cooling_rate, t_temperature, configuration);
    fprintf(fp, "%f\n", omp_get_wtime() - time);
    printf("%f\n", omp_get_wtime() - time);
	//fprintf(fp, "Jobs: %d Machines: %d\n", JOBS, MACHINES);
	//fprintf(fp, "%d\n", cost);
	//printf("Duration:%f\n", omp_get_wtime() - time);
    fclose(fp);
    
    return 0;
}
