/*
 * TAREA 4 - Práctica Nº 2: Introducción a OpenMP
 * Universidad Nacional de San Antonio Abad del Cusco
 * Algoritmos Paralelos y Distribuidos
 *
 * Programa: Suma paralela de un vector de N=1000 enteros con OpenMP
 * Compilar: g++ -fopenmp -o tarea4 tarea4_openmp.cpp
 * Ejecutar: ./tarea4
 */

#include <iostream>
#include <cstdlib>   // rand(), srand()
#include <ctime>     // time()
#include <omp.h>

using namespace std;

int main()
{
    const int N = 1000;
    int vector[N];
    int suma_total = 0;   // Variable COMPARTIDA para la suma total

    // 1. Inicializar el vector con valores enteros aleatorios (1 - 100)
    srand((unsigned int)time(NULL));
    for (int k = 0; k < N; k++)
    {
        vector[k] = rand() % 100 + 1;
    }

    cout << "Vector inicializado con " << N << " valores aleatorios (1-100).\n" << endl;

    // 2. Paralelizar el cálculo de la suma usando #pragma omp parallel
    //    con 4 hilos
#pragma omp parallel num_threads(4) shared(suma_total, vector)
    {
        int suma_parcial = 0;   // Variable PRIVADA: cada hilo tiene la suya

        int tid = omp_get_thread_num();
        int nth = omp_get_num_threads();

        // Dividir el vector entre los hilos de forma manual
        // Cada hilo procesa una porción del vector
        int elementos_por_hilo = N / nth;
        int inicio = tid * elementos_por_hilo;
        int fin = (tid == nth - 1) ? N : inicio + elementos_por_hilo;
        // El último hilo se lleva el resto si N no es divisible entre nth

        // Cada hilo calcula su suma parcial
        for (int i = inicio; i < fin; i++)
        {
            suma_parcial += vector[i];
        }

        // Mostrar información del hilo
#pragma omp critical
        {
            cout << "Hilo " << tid
                << " | elementos [" << inicio << " - " << (fin - 1) << "]"
                << " | suma parcial = " << suma_parcial << endl;

            // Acumular la suma parcial en la suma total (sección crítica)
            suma_total += suma_parcial;
        }

    } // Fin de la región paralela

    // 3. Mostrar la suma total final
    cout << "\nSuma total = " << suma_total << endl;

    return 0;
}