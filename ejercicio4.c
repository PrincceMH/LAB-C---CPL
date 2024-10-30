#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 4 // Asumiendo una matriz de 4x4 para simplicidad

int main(int argc, char* argv[]) {
    int comm_sz, my_rank;
    double matrix[N][N], vector[N], local_matrix[N], local_result = 0;
    double result[N] = {0};

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        // Inicializar la matriz y el vector a multiplicar
        for (int i = 0; i < N; i++) {
            vector[i] = i + 1; // Valores de ejemplo para el vector
            for (int j = 0; j < N; j++) {
                matrix[i][j] = i + j + 1; // Valores de ejemplo para la matriz
            }
        }
    }

    // Transmitir el vector a todos los procesos
    MPI_Bcast(vector, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Esparcir las filas de la matriz a todos los procesos
    MPI_Scatter(matrix, N, MPI_DOUBLE, local_matrix, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Cada proceso calcula su parte de la multiplicación matriz-vector
    local_result = 0;
    for (int j = 0; j < N; j++) {
        local_result += local_matrix[j] * vector[j];
    }

    // Recoger los resultados parciales para formar el vector resultado final en el proceso 0
    MPI_Gather(&local_result, 1, MPI_DOUBLE, result, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        // El proceso 0 imprime el vector resultante
        printf("Vector resultante:\n");
        for (int i = 0; i < N; i++) {
            printf("%f\n", result[i]);
        }
    }

    MPI_Finalize();
    return 0;
}

