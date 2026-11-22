#include <iostream>
#include <thread>
#include <string>
#include <Windows.h>
using namespace std;
void imprime(string palabra, long tiempo)
{
	for (int i = 1; i <= 100; i++)
	{
		cout << palabra << '\n';
		Sleep(tiempo); //--duerme en milisegundos
	}
}
int main()
{
	// Crea y ejecuta hilo0 e hilo1
	thread hilo0(imprime, "tin", 10);
	thread hilo1(imprime, "ton", 5);

	hilo0.join();
	hilo1.join();

	cout << "termino la ejecución de los hilos.." << '\n';
	getchar();
	return 0;
}