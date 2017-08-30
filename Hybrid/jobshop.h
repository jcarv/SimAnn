extern int JOBS, MACHINES, SWAPS;
extern unsigned int seed;

void my_free(int** configuration);
int **alloc_2d_int();
double randZeroToOne();
double probability(double delta, double temperature);
void copy_matrix(int** source, int** destination);
void generate_solution(int** solution, int** configuration);
void generate_neighbour(int** neighbour, int** solution, int** configuration);
void output_configuration(int** configuration);
void load_configuration(int** configuration);
int cost(int** solution);

