#include <iostream>
#include <thread>
#include <vector>
using namespace std;

// Suma los elementos de un vector y guarda el resultado en res[idx]
void sumaVector(const vector<int>& v, long long* res, int idx) {
    long long s = 0;
    for (int x : v) {
        s += x;
    }
    res[idx] = s;
}

int main() {
    vector<int> v1 = { 1, 2, 3, 4, 5 };
    vector<int> v2 = { 10, 20, 30 };
    vector<int> v3 = { 100, 200, 300, 400, 500, 600 };

    long long resultados[3] = { 0, 0, 0 };

    // Crear y lanzar los tres hilos
    thread t0(sumaVector, ref(v1), resultados, 0);
    thread t1(sumaVector, ref(v2), resultados, 1);
    thread t2(sumaVector, ref(v3), resultados, 2);

    t0.join();
    t1.join();
    t2.join();

    long long total = resultados[0] + resultados[1] + resultados[2];

    cout << "Suma v1: " << resultados[0] << endl;
    cout << "Suma v2: " << resultados[1] << endl;
    cout << "Suma v3: " << resultados[2] << endl;
    cout << "Suma total: " << total << endl;

    return 0;
}