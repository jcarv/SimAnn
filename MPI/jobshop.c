#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "jobshop.h"

void my_free(int** configuration) {
	free(configuration[0]);
	free(configuration);
}


int **alloc_2d_int() {
    int *data = (int *)malloc(JOBS*MACHINES*sizeof(int));
    int **array= (int **)malloc(JOBS*sizeof(int*));
    for (int i=0; i<JOBS; i++)
        array[i] = &(data[MACHINES*i]);

    return array;
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

void copy_matrix(int** source, int** destination) {
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

void generate_solution(int** solution, int** configuration) {
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

void generate_neighbour(int** neighbour, int** solution, int** configuration) {
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

void output_configuration(int** configuration) {
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

void load_configuration(int** configuration) {
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

int cost(int** solution) {
    int res = 0;

    for(int i = 0; i < JOBS; i++){
        for(int j = 0; j < MACHINES; j++){
            res += solution[i][j];
        }
    }
    
    return res;
}
