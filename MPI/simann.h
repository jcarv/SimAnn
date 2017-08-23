extern int FROZEN;

void simulated_annealing(int** current_solution, int** configuration);
void shared_simulated_annealing(int **best_guess, int exchange_parameter, int final_exchange, int** current_solution, int** configuration);
