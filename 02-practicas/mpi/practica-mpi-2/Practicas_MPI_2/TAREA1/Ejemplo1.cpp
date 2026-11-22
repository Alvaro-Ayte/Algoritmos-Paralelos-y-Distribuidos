
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 12   

int main(int argc, char** argv)
{
    int rank, npr;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &npr);

    int chunk = N / npr;          
    int* subvector = (int*)malloc(chunk * sizeof(int));

    //  rank == 0
    if (rank == 0)
    {
        // Crear el vector completo con valores aleatorios
        int* vector = (int*)malloc(N * sizeof(int));
        srand(42);
        printf("\n[Maestro] Vector generado (N=%d):\n", N);
        for (int i = 0; i < N; i++)
        {
            vector[i] = rand() % 100 + 1;   // valores entre 1 y 100
            printf("  vector[%2d] = %d\n", i, vector[i]);
        }
        printf("\n");

        for (int j = 0; j < chunk; j++)
            subvector[j] = vector[j];
        for (int dest = 1; dest < npr; dest++)
            MPI_Send(&vector[dest * chunk], chunk, MPI_INT,dest, 0, MPI_COMM_WORLD);

        free(vector);
    }
    //  PROCESOS ESCLAVOS
    else
    {
        MPI_Recv(subvector, chunk, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int suma = 0;
    for (int j = 0; j < chunk; j++)
        suma += subvector[j];

    printf("[Proceso %d de %d]  Suma parcial de %d elementos = %d\n", rank, npr, chunk, suma);
    free(subvector);
    MPI_Finalize();
    return 0;
}