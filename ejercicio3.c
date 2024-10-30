#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    int comm_sz, my_rank;
    int local_value, sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Cada proceso inicializa su valor local (por ejemplo, el rango + 1)
    local_value = my_rank + 1;
    int step = 1;
    
    // Realizar la reducción estructurada en forma de árbol
    while (step < comm_sz) {
        if (my_rank % (2 * step) == 0) {
            if (my_rank + step < comm_sz) {
                int received_value;
                // Recibir el valor del proceso vecino
                MPI_Recv(&received_value, 1, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_value += received_value;
            }
        } else {
            // Enviar el valor al proceso vecino
            MPI_Send(&local_value, 1, MPI_INT, my_rank - step, 0, MPI_COMM_WORLD);
            break; // Después de enviar, el proceso ha terminado
        }
        step *= 2; // Incrementar el tamaño del paso
    }

    if (my_rank == 0) {
        // El proceso 0 imprime la suma total
        printf("Suma total: %d\n", local_value);
    }

    MPI_Finalize();
    return 0;
}
