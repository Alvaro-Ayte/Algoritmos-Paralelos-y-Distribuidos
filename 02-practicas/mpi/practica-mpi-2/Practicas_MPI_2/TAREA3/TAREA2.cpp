
#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv)
{
    int rank, size;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2)
    {
        if (rank == 0)
            printf("ERROR: Este programa requiere al menos 2 procesos.\n");
        MPI_Finalize();
        return 1;
    }
    double numeros[2];  
    double suma;

    //  envia dos reales y espera el resultado 
    if (rank == 0)
    {
        numeros[0] = 3.75;
        numeros[1] = 8.50;
        printf("[Proceso 0] Enviando al proceso 1:  a = %.2f  |  b = %.2f\n",    numeros[0], numeros[1]);

        MPI_Send(numeros, 2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);

        MPI_Recv(&suma, 1, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD, &status);

        printf("[Proceso 0] Resultado recibido:  %.2f + %.2f = %.2f\n",  numeros[0], numeros[1], suma);
    }
    // ---- PROCESO 1: recibe, calcula y devuelve la suma ----
    else if (rank == 1)
    {
        MPI_Recv(numeros, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

        suma = numeros[0] + numeros[1];

        printf("[Proceso 1] Recibi:  a = %.2f  |  b = %.2f  =>  suma = %.2f\n",
            numeros[0], numeros[1], suma);

        // Devolver la suma al proceso 0
        MPI_Send(&suma, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}