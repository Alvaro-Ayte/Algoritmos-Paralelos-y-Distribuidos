#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <iostream>

int main(int argc, char* argv[])
{
	int my_rank;
	int n;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if (my_rank == 0)
	{
		printf("Hola, ingresa un número: ");
		std::cin >> n;
	}
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	printf("Hola, soy el proceso %d y el número es el %d\n", my_rank, n);
	MPI_Finalize();

}