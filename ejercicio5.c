#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 16 // Número total de claves

// Función de comparación para qsort
int cmpfunc(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// Función de fusión para fusionar dos arreglos ordenados
void merge(int** local_keys, int* local_n, int* other_keys, int other_n) {
    int* temp = malloc((*local_n + other_n) * sizeof(int));
    int i = 0, j = 0, k = 0;
    
    // Fusionar los dos arreglos ordenados
    while (i < *local_n && j < other_n) {
        if ((*local_keys)[i] <= other_keys[j]) {
            temp[k++] = (*local_keys)[i++];
        } else {
            temp[k++] = other_keys[j++];
        }
    }
    
    // Copiar los elementos restantes de local_keys
    while (i < *local_n) {
        temp[k++] = (*local_keys)[i++];
    }
    
    // Copiar los elementos restantes de other_keys
    while (j < other_n) {
        temp[k++] = other_keys[j++];
    }
    
    free(*local_keys);  // Liberar la memoria original de local_keys
    *local_keys = temp; // Asignar el nuevo arreglo fusionado a local_keys
    *local_n += other_n; // Actualizar el tamaño de local_keys
}

int main(int argc, char* argv[]) {
    int comm_sz, my_rank;
    int local_n;
    int* keys;
    int* local_keys;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Calcular el número de claves que cada proceso manejará
    local_n = N / comm_sz;
    local_keys = malloc(local_n * sizeof(int));

    if (my_rank == 0) {
        // El proceso 0 inicializa el arreglo de claves con valores aleatorios
        keys = malloc(N * sizeof(int));
        for (int i = 0; i < N; i++) {
            keys[i] = rand() % 100;
        }
    }

    // Esparcir las claves del proceso 0 a todos los procesos
    MPI_Scatter(keys, local_n, MPI_INT, local_keys, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Ordenar las claves locales usando qsort
    qsort(local_keys, local_n, sizeof(int), cmpfunc);

    // Realizar la fusión estructurada en forma de árbol
    int step = 1;
    while (step < comm_sz) {
        if (my_rank % (2 * step) == 0) {
            if (my_rank + step < comm_sz) {
                int other_n;
                // Recibir el número de claves del proceso vecino
                MPI_Recv(&other_n, 1, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int* other_keys = malloc(other_n * sizeof(int));
                // Recibir las claves del proceso vecino
                MPI_Recv(other_keys, other_n, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // Fusionar las claves recibidas con las claves locales
                merge(&local_keys, &local_n, other_keys, other_n);
                free(other_keys);
            }
        } else {
            // Enviar el número de claves y las claves al proceso vecino
            MPI_Send(&local_n, 1, MPI_INT, my_rank - step, 0, MPI_COMM_WORLD);
            MPI_Send(local_keys, local_n, MPI_INT, my_rank - step, 0, MPI_COMM_WORLD);
            break; // Después de enviar, el proceso ha terminado
        }
        step *= 2; // Incrementar el tamaño del paso
    }

    if (my_rank == 0) {
        // El proceso 0 imprime las claves ordenadas
        printf("Claves ordenadas:\n");
        for (int i = 0; i < local_n; i++) {
            printf("%d ", local_keys[i]);
        }
        printf("\n");
        free(keys);
    }

    free(local_keys);
    MPI_Finalize();
    return 0;
}

