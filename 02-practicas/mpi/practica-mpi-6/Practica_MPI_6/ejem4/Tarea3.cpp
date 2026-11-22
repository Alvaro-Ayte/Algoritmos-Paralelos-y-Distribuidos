/*
 * PRACTICA 10 - COMUNICACION COLECTIVA MPI
 * TRABAJO 4.1: Calculo de Integral Doble Gaussiana 2D
 *
 * Integral: integral(-inf,+inf) integral(-inf,+inf) e^(-(x^2+y^2)/2) dx dy
 *
 * Aproximacion numerica:
 *   Sum(i=0..N) Sum(j=0..M) h^2 * e^(-(xi^2 + yj^2)/2)
 *
 * Donde:
 *   xi = -xN + i*h,   i = 0,...,N
 *   yj = -yM + j*h,   j = 0,...,M
 *
 * Valor exacto: 2*PI ≈ 6.283185307179586
 *
 * Estrategia de paralelismo:
 *   - Proceso 0 lee N, M y los difunde con MPI_Bcast
 *   - Cada proceso calcula su porcion de filas (dimension i) con stride = size
 *   - Se suman los resultados parciales con MPI_Reduce (MPI_SUM)
 */
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "mpi.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    int N, M;               // Numero de puntos en X e Y
    int rank, size;         // Rango e identificador del proceso
    double h;               // Paso de la cuadricula
    double xN, yM;          // Limites de integracion truncados
    double local_sum;       // Suma parcial de cada proceso
    double global_sum;      // Suma total (resultado final)
    double xi, yj;          // Coordenadas del punto de cuadratura
    double t_inicio, t_fin; // Para medir tiempo (speed-up)

    /* Valor exacto de la integral: 2*pi */
    const double VALOR_EXACTO = 2.0 * M_PI;

    /* ----------------------------------------------------------
     * 1. Inicializacion MPI
     * ---------------------------------------------------------- */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* ----------------------------------------------------------
     * 2. Solo el proceso 0 lee los parametros N y M
     * ---------------------------------------------------------- */
    if (rank == 0)
    {
        cout << "=== Calculo de Integral Gaussiana 2D con MPI ===" << endl;
        cout << "Numero de procesos: " << size << endl;
        cout << "Ingresa N (puntos en X, ej: 1000): ";
        cin >> N;
        cout << "Ingresa M (puntos en Y, ej: 1000): ";
        cin >> M;

        if (N <= 0 || M <= 0)
        {
            cerr << "Error: N y M deben ser mayores a 0." << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    /* ----------------------------------------------------------
     * 3. Broadcast de N y M a todos los procesos
     *    (igual que en el Ejercicio 6 con Bcast para n)
     * ---------------------------------------------------------- */
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* ----------------------------------------------------------
     * 4. Calculo de parametros de la cuadricula
     *
     *    El dominio se trunca a [-xN, +xN] x [-yM, +yM]
     *    donde xN = yM = 6 es suficiente (e^(-18) ≈ 0 en los extremos)
     *    h es el paso uniforme en ambas dimensiones
     * ---------------------------------------------------------- */
    xN = 6.0;   /* limite de truncamiento en X */
    yM = 6.0;   /* limite de truncamiento en Y */
    h = (2.0 * xN) / (double)N;  /* paso de cuadricula */

    /* Sincronizamos antes de medir tiempo */
    MPI_Barrier(MPI_COMM_WORLD);
    t_inicio = MPI_Wtime();

    /* ----------------------------------------------------------
     * 5. Cada proceso calcula su porcion de la suma doble
     *
     *    Distribucion: cada proceso toma las filas i donde
     *    i % size == rank  (igual al stride del Ejercicio 6 con PI)
     *
     *    xi = -xN + i*h
     *    yj = -yM + j*h
     *    Contribucion = h^2 * e^(-(xi^2 + yj^2)/2)
     * ---------------------------------------------------------- */
    local_sum = 0.0;

    for (int i = rank; i <= N; i += size)
    {
        xi = -xN + i * h;          /* coordenada x del punto i */

        for (int j = 0; j <= M; j++)
        {
            yj = -yM + j * h;      /* coordenada y del punto j */

            /* Funcion del integrando: e^(-(x^2+y^2)/2) */
            local_sum += exp(-(xi * xi + yj * yj) / 2.0);
        }
    }

    /* Multiplicamos por h^2 para obtener el area */
    local_sum *= h * h;

    /* ----------------------------------------------------------
     * 6. Reduccion: todos los procesos suman sus resultados
     *    parciales en el proceso 0 (igual que en Ejercicio 4 y 6)
     * ---------------------------------------------------------- */
    MPI_Reduce(&local_sum,    /* valor local a reducir        */
        &global_sum,  /* donde se guarda el resultado */
        1,            /* numero de datos              */
        MPI_DOUBLE,   /* tipo de dato                 */
        MPI_SUM,      /* operacion: suma              */
        0,            /* proceso raiz que recibe      */
        MPI_COMM_WORLD);

    /* Barrera para medir tiempo correctamente */
    MPI_Barrier(MPI_COMM_WORLD);
    t_fin = MPI_Wtime();

    /* ----------------------------------------------------------
     * 7. Solo el proceso 0 imprime resultados
     *    (igual que en Ejercicio 6 con calculo de PI)
     * ---------------------------------------------------------- */
    if (rank == 0)
    {
        double error = fabs(global_sum - VALOR_EXACTO);
        double tiempo = t_fin - t_inicio;

        cout << "\n--- RESULTADOS ---" << endl;
        cout << "N = " << N << ",  M = " << M << endl;
        cout << "Paso h         = " << h << endl;
        cout << "Valor exacto   = " << VALOR_EXACTO << endl;
        cout << "Valor calculado= " << global_sum << endl;
        cout << "Error absoluto = " << error << endl;
        cout << "Tiempo elapsed = " << tiempo << " segundos" << endl;
        cout << "Procesos usados= " << size << endl;
    }

    /* ----------------------------------------------------------
     * 8. Finalizamos MPI
     * ---------------------------------------------------------- */
    MPI_Finalize();
    return 0;
}