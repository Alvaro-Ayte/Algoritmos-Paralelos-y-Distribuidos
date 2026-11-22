#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

// Hilo: encuentra el máximo en [inicio, fin)
void maxSegmento(const vector<int>& v,
    int inicio, int fin,
    int* maxLocal) {
    int m = INT_MIN;
    for (int i = inicio; i < fin; i++)
        if (v[i] > m) m = v[i];
    *maxLocal = m;
}

int main() {
    vector<int> datos = {
        42, 17, 93, 55, 28, 76, 8, 64,
        31, 99, 47, 12, 83, 61, 25, 70
    };

    const int N = 4;   // número de hilos 
    int tam = datos.size();
    int porParte = tam / N;

    vector<thread> hilos(N);
    vector<int>  maxLocales(N, INT_MIN);

    // Lanzar N hilos, cada uno sobre su segmento
    for (int k = 0; k < N; k++) {
        int ini = k * porParte;
        int fin = (k == N - 1) ? tam : ini + porParte;
        hilos[k] = thread(maxSegmento,
            ref(datos), ini, fin,
            &maxLocales[k]);
    }

    // Esperar a todos
    for (int k = 0; k < N; k++)
        hilos[k].join();

    // Hilo principal: máximo global
    int maxGlobal = *max_element(
        maxLocales.begin(),
        maxLocales.end());

    cout << "Máximos locales por segmento:\n";
    for (int k = 0; k < N; k++)
        cout << "  Segmento " << k << ": "
        << maxLocales[k] << endl;

    cout << "Máximo global: " << maxGlobal << endl;

    return 0;
}