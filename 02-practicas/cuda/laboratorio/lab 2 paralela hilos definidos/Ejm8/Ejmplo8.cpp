#include <iostream>
#include <omp.h>
using namespace std;
int main()
{
	int tid, nth;
#pragma omp parallel private(tid)
	{
		tid = omp_get_thread_num();
		nth = omp_get_num_threads();
		//printf("Este es el hilo %d de un total de %d\n", tid, nth);
		std::cout << "Hola mundo desde el hilo "
			<< tid
			<< " de un total de "
			<< nth
			<< std::endl;
	}
	getchar();
	return 0;
}