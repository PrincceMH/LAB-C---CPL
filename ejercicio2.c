#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    int comm_sz, my_rank;
    long long int number_of_tosses, local_tosses, number_in_circle = 0, local_number_in_circle = 0;
    double x, y, distance_squared;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        // El proceso 0 lee el número total de lanzamientos de dardos del usuario
        printf("Ingrese el número de lanzamientos: ");
        scanf("%lld", &number_of_tosses);
    }
    
    // Transmitir el número de lanzamientos a todos los procesos
    MPI_Bcast(&number_of_tosses, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    // Cada proceso calcula su parte del número total de lanzamientos
    local_tosses = number_of_tosses / comm_sz;
    srand(time(NULL) + my_rank); // Sembrar el generador de números aleatorios de manera diferente para cada proceso

    // Realizar la simulación local de Monte Carlo
    for (long long int toss = 0; toss < local_tosses; toss++) {
        x = (double)rand() / RAND_MAX * 2.0 - 1.0; // Coordenada x aleatoria entre -1 y 1
        y = (double)rand() / RAND_MAX * 2.0 - 1.0; // Coordenada y aleatoria entre -1 y 1
        distance_squared = x * x + y * y;
        if (distance_squared <= 1) local_number_in_circle++;
    }

    // Reducir las cuentas locales en la cuenta global en el proceso 0
    MPI_Reduce(&local_number_in_circle, &number_in_circle, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        // El proceso 0 calcula e imprime la estimación de Pi
        double pi_estimate = 4 * number_in_circle / ((double)number_of_tosses);
        printf("Valor estimado de Pi: %lf\n", pi_estimate);
    }

    MPI_Finalize();
    return 0;
}
