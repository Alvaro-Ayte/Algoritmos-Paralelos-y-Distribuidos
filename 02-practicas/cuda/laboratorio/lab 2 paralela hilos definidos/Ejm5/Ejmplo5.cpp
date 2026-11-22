#include <iostream>
#include <omp.h>
using namespace std;
int main()
{// saret(una privda a global)
	//private (una global a privada)
	int i; // comprate errores y esta en la memoira variable compratida 
#pragma omp parallel num_threads(4)
	{
		std::cout << "Imprime ciclo...!\n" << std::endl;
		//int i; //--variable local pribada para cada hilo
		//std::cout << i <<"hoañañañañña" << std::endl;
		for (i = 0; i < 10; i++)
		{
			std::cout << "unsaac " << i << std::endl;
		}
	}
std::cout << "Fin de la impresión de ciclos...!\n" << std::endl;
getchar();
return 0;
}