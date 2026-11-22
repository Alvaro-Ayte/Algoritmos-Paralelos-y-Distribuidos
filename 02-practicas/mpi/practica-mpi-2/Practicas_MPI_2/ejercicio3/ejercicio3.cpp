#include <stdio.h>
#include <string.h>
#include "mpi.h"
int main(int argc, char* argv[])
{
	int i, j, k, l;
	int iarray1[16];
	int iarray2[4];
	int myid, numprocs, ierr;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	printf("soy el nodo %d de %d procesos\n", myid, numprocs);
	for (i = 0; i < 16; i++)
		iarray1[i] = 0;
	for (j = 0; j < 4; j++)
		iarray2[j] = (myid * 4) + j;
	MPI_Gather(iarray2, 4, MPI_INTEGER, iarray1, 4,
		MPI_INTEGER, 0, MPI_COMM_WORLD);
	if (myid == 0)
		for (i = 0; i < 16; i++)
			printf("%d\n", iarray1[i]);

	MPI_Finalize();

}