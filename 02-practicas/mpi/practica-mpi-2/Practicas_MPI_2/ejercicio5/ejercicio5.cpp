#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
int main(int argc, char** argv)
{
	int p, np, i;
	int* sendarray, * recvarray;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	sendarray = (int*)malloc(np * sizeof(int));
	recvarray = (int*)malloc(np * sizeof(int));
	printf("proc(%d): sendarray={ ", p);
	for (i = 0; i < np; i++) {
		sendarray[i] = 10 * p + i;
		printf("%d ", sendarray[i]);
	}

	printf("}\n"); fflush(stdout);
	//system("sleep 5");
	MPI_Alltoall(sendarray, 1, MPI_INT, recvarray, 1, MPI_INT, MPI_COMM_WORLD);
	printf("proc(%d): recvarray={ ", p);
	for (i = 0; i < np; i++) {
		printf("%d ", recvarray[i]);
	}
	printf("}\n"); fflush(stdout);
	MPI_Finalize();
}