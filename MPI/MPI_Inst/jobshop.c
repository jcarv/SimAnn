#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "jobshop.h"

double T_COMP;

void my_free(int** configuration) {
    double t1,t2;
    t1 = MPI_Wtime();

	free(configuration[0]);
    free(configuration);

    t2 = MPI_Wtime();

    T_COMP += t2-t1;
}


int **alloc_2d_int() {
    double t1,t2;
    t1 = MPI_Wtime();

    int *data = (int *)malloc(JOBS*MACHINES*sizeof(int));
    int **array= (int **)malloc(JOBS*sizeof(int*));
    for (int i=0; i<JOBS; i++)
        array[i] = &(data[MACHINES*i]);

    t2 = MPI_Wtime();

    T_COMP += t2-t1;

    return array;
}

double randZeroToOne()
{
    double t1,t2;
    t1 = MPI_Wtime();

    int res = rand_r(&seed) / (RAND_MAX + 1.);;

    t2 = MPI_Wtime();
    
    T_COMP += t2-t1;
    
    return res;
}

double probability(double delta, double temperature) {
    double t1,t2;
    t1 = MPI_Wtime();

    double res = exp(-delta/temperature);
    
    if (res >= 1) {
        res = 1;
    }

    t2 = MPI_Wtime();
    
    T_COMP += t2-t1;

    return res;
}

void copy_matrix(int** source, int** destination) {
    double t1,t2;
    t1 = MPI_Wtime();

    int i = 0, j = 0;
    
    while(i < JOBS) {
        j = 0;
        while(j < MACHINES) {
            destination[i][j] = source[i][j];
            j++;
        }
        i++;
    }

    t2 = MPI_Wtime();
    
    T_COMP += t2-t1;
}

void generate_solution(int** solution, int** configuration) {
    copy_matrix(configuration, solution);

    double t1,t2;
    t1 = MPI_Wtime();

    for(int i = 0; i < JOBS; i++){
        int machine = rand_r(&seed) % MACHINES;
        //solution[i][machine] = configuration[i][machine];

        for(int k = 0; k < MACHINES; k++){
            if(machine != k){
                solution[i][k] = 0;
            }
        }
    }

    t2 = MPI_Wtime();
    
    T_COMP += t2-t1;
}

void generate_neighbour(int** neighbour, int** solution, int** configuration) {
    copy_matrix(solution, neighbour);

    double t1,t2;
    t1 = MPI_Wtime();

    int original_machine = 0;
    int job_swap;
    
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

    t2 = MPI_Wtime();
    
    T_COMP += t2-t1;
}

void output_configuration(int** configuration) {
    double t1,t2;
    t1 = MPI_Wtime();

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
    
    t2 = MPI_Wtime();

    T_COMP += t2-t1;
}

void load_configuration(int** configuration) {
    double t1,t2;
    t1 = MPI_Wtime();

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

    t2 = MPI_Wtime();

    T_COMP += t2-t1;
}

int cost(int** solution) {
    double t1,t2;
    t1 = MPI_Wtime();

    int res = 0;

    for(int i = 0; i < JOBS; i++){
        for(int j = 0; j < MACHINES; j++){
            res += solution[i][j];
        }
    }
    
    t2 = MPI_Wtime();

    T_COMP += t2-t1;

    return res;
}
