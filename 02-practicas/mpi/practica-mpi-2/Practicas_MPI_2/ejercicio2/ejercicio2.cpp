#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include "mpi.h"
int main(int argc, char** argv)
{
	int p, np;
	int nAtoms;
	int* Distribucion = NULL;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	if (p == 0)
	{
		srand(7654321);
		Distribucion = (int*)malloc(np * sizeof(int));
		for (p = 0; p < np; p++)
		{
			Distribucion[p] = rand();
		}
	}
	MPI_Scatter(Distribucion, 1, MPI_INT, &nAtoms, 1, MPI_INT, 0, MPI_COMM_WORLD);
	printf("En proceso %d hay %d atomos\n", p, nAtoms);
	if (p == 0)
		delete(Distribucion);
	MPI_Finalize();
}