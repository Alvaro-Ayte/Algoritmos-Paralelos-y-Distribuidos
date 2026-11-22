#include <iostream>
#include <thread>
#include <vector>
using namespace std;

const int N = 4;

void sumaFilas(const vector<vector<int>>& mat, vector<long long>& sumaF) {
    for (int i = 0; i < N; i++) {
        sumaF[i] = 0;
        for (int j = 0; j < N; j++)
            sumaF[i] += mat[i][j];
    }
}

void sumaColumnas(const vector<vector<int>>& mat, vector<long long>& sumaC) {
    for (int j = 0; j < N; j++) {
        sumaC[j] = 0;
        for (int i = 0; i < N; i++)
            sumaC[j] += mat[i][j];
    }
}

int main() {
    vector<vector<int>> matriz = {
        { 1,  2,  3,  4},
        { 5,  6,  7,  8},
        { 9, 10, 11, 12},
        {13, 14, 15, 16}
    };

    vector<long long> sumaF(N, 0);
    vector<long long> sumaC(N, 0);

    thread tFilas(sumaFilas, ref(matriz), ref(sumaF));
    thread tCols(sumaColumnas, ref(matriz), ref(sumaC));

    tFilas.join();
    tCols.join();

    cout << "Suma por filas:\n";
    for (int i = 0; i < N; i++)
        cout << "  Fila " << i << ": " << sumaF[i] << endl;

    cout << "Suma por columnas:\n";
    for (int j = 0; j < N; j++)
        cout << "  Col " << j << ": " << sumaC[j] << endl;

    return 0;
}