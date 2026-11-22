#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define N 4
#define M 7
//--enviando una columna
int main(int argc, char* argv[])
{
	int numprocs, rank, orig = 0, dest = 1, tag = 1;
	int A[N][M] =
	{ 3,4,9,2,4,7,1,
	6,3,0,7,2,9,8,
	2,5,1,4,3,2,7,
	8,5,6,2,7,6,3 };
	int B[N];
	MPI_Status stat;
	MPI_Datatype columna;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Type_vector(N, //--num. de elementos en una columna
		1, //--un elemento por fila
		M, //--tomar un elemento cada M (stride
		MPI_INT, &columna);
	MPI_Type_commit(&columna);
	if (rank == orig)
	{
		MPI_Send(&A[0][3], 1, columna, dest, tag, MPI_COMM_WORLD);
	}
	else
	{
		if (rank == dest)
		{
			MPI_Recv(&B[0], N, MPI_INT, orig, tag, MPI_COMM_WORLD, &stat);
			printf("rank= %d b= %3.1d %3.1d %3.1d %3.1d\n", rank, B[0], B[1], B[2], B[3]);
		}
	}
	MPI_Type_free(&columna);
	MPI_Finalize();
}