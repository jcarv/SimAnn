#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

int main(char **args){
	int numJobs,numMachines,i,j,tempo;
	unsigned int seed = time(NULL);
	char filename[200] = "config";
	char jobs[20];
	char machines[20];

    printf("Numero de jobs: ");
    scanf("%d",&numJobs);
    printf("Numero de machines: ");
    scanf("%d",&numMachines);

	sprintf(jobs, "%d", numJobs);
	sprintf(machines, "%d", numMachines);

	strcat(filename, jobs);
	strcat(filename, "x");
	strcat(filename, machines);
	strcat(filename, ".txt");

	FILE *fp = fopen(filename, "w");
	if (fp == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	fprintf(fp, "%d %d\n",numJobs,numMachines);

    for(i = 1;i <= numJobs; i++){
        for(j = 1; j <= numMachines; j++){
            //rand() % (max_number + 1 - minimum_number) + minimum_number
            tempo = rand_r(&seed) % (11 - 1) + 1;
            fprintf(fp, "%d %d %d\n",i,j,tempo);
        }
    }

	fclose(fp);

	printf("Configuração gerada com sucesso.\n");

	return 0;
}
