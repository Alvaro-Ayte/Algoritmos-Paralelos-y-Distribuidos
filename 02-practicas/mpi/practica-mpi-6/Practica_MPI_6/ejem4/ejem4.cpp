#include "mpi.h"
#include <stdio.h>
#define NELEMENTS 6
int main(int argc, char* argv[]) {
	int numtasks, rank, source = 0, dest, tag = 1, i;
	int blocklengths[2], displacements[2];
	float a[16] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0,
	14.0, 15.0, 16.0 };
	float b[NELEMENTS];
	MPI_Status stat;
	MPI_Datatype indextype; // nuevo tipo de dato
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	blocklengths[0] = 4; //-- toma 4 elementos del arreglo
	blocklengths[1] = 2; //-- toma 2 elementos del arreglo
	displacements[0] = 5;//-- comienza del elemento de indice 5 el primer bloque que es 6.0
	displacements[1] = 12;//-- comienza del elemento de indice 12 que es 13.0
	//--crea el tipo de dato derivado indexado
	MPI_Type_indexed(2, blocklengths, displacements, MPI_FLOAT, &indextype);
	MPI_Type_commit(&indextype);
	if (rank == 0) {
		MPI_Send(a, 1, indextype, 1, tag, MPI_COMM_WORLD);
	}
	else
	{
		if (rank == 1)
		{
			MPI_Recv(b, NELEMENTS, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &stat);
			printf("rank= %d b= %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f\n",
				rank, b[0], b[1], b[2], b[3], b[4], b[5]);
		}
	}
	//--libera los tipos de datos
	MPI_Type_free(&indextype);
	MPI_Finalize();
}