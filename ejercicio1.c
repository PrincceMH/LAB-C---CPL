#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int comm_sz, my_rank;
    int n = 100; // Número de elementos en los datos
    int local_n;
    int *data = NULL;
    int *local_data;
    int histogram[10] = {0}; // Asumiendo 10 intervalos
    int local_histogram[10] = {0};

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Calcular el número de elementos que cada proceso manejará
    local_n = n / comm_sz;
    local_data = malloc(local_n * sizeof(int));

    if (my_rank == 0) {
        // El proceso 0 inicializa el arreglo de datos con valores aleatorios entre 0 y 99
        data = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            data[i] = rand() % 100;
        }
    }

    // Esparcir los datos del proceso 0 a todos los procesos, incluido el mismo
    MPI_Scatter(data, local_n, MPI_INT, local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular el histograma local
    for (int i = 0; i < local_n; i++) {
        int bin = local_data[i] / 10; // Determinar en qué intervalo cae el valor
        local_histogram[bin]++;
    }

    // Reducir todos los histogramas locales en el histograma global en el proceso 0
    MPI_Reduce(local_histogram, histogram, 10, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        // El proceso 0 imprime el histograma final
        printf("Histograma:\n");
        for (int i = 0; i < 10; i++) {
            printf("Intervalo %d: %d\n", i, histogram[i]);
        }
        free(data);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
