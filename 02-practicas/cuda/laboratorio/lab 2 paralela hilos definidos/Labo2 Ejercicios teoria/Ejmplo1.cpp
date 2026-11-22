#include <iostream>
#include <omp.h>
int main()				
{
	#pragma omp parallel
	{	
		//std::cout << "Hola...!\n" << std::endl;
		std::cout << "Hola...!\n" << std::endl;
		
	}
	std::cout << "Adios...!\n" << std::endl;
	getchar();
	return 0;
}