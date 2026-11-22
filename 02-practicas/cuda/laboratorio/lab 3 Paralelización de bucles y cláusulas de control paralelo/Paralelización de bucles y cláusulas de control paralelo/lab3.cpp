#include <stdio.h>
#include <omp.h>

int main() {
    long long n;
    long long suma = 0;

    printf("Ingrese un número entero positivo n: ");
    scanf("%lld", &n);

    if (n <= 0) {
        printf("Error: el número debe ser positivo.\n");
        return 1;
    }

#pragma omp parallel for schedule(dynamic, 2) reduction(+:suma) num_threads(8)
    for (long long i = 1; i <= n; i++) {
        suma += i;
        printf("Hilo %d sumando el número %lld\n", omp_get_thread_num(), i);
    }

    printf("\nLa suma de los primeros %lld números naturales es: %lld\n", n, suma);

    // Verificación con fórmula matemática
    long long esperado = n * (n + 1) / 2;
    printf("Verificación (n*(n+1)/2): %lld\n", esperado);

    return 0;
}