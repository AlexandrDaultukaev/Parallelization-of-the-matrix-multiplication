/* Умножение матриц с использованием OpenMP */

#include "CLI11.hpp"
#include <omp.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <fcntl.h>
#include <string.h>

/* Выделение памяти под матрицы размером size*size */
double *alloc_matrix(int size) {
    double *matrix = (double*)malloc(size * size * sizeof(double));
    printf("matrix %dx%d allocated\n", size, size);
    return matrix;
}

/* Освобождение памяти матрицы */
void del_matrix(double *matrix) {
    free(matrix);
}

/* Функция для измерения времени работы */
double dtime() {
    struct timeval tv;
    struct timezone tz;
    double t;
    gettimeofday(&tv, &tz);
    t = (double)tv.tv_sec + (double)tv.tv_usec*1e-6;
    return(t);
}

int main(int argc, char** argv) {
    int N = 50; // Размер матрицы
    int THR = 2; // Число потоков
    int i, j, k;

    CLI::App app{"App description"};
    app.add_option("-n", N, "Size");
    app.add_option("-t", THR, "Number of threads");
    CLI11_PARSE(app, argc, argv);

    /*
       Устанавливаем, что среда выполнения
       не может динамически изменять количество
       потоков
    */
    omp_set_dynamic(0);

    /* Задаем количество потоков */
    omp_set_num_threads(THR);

    /* Выделение памяти */
    double *A = alloc_matrix(N);
    double *B = alloc_matrix(N);
    double *C = alloc_matrix(N);

    srand(time(NULL));

    /* Инициализация матриц значениями */
    #pragma omp parallel for shared(A, B) private(i, j)
    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
            A[i + j * N] = rand() % 5;
            B[i + j * N] = rand() % 5;
        }
    }

    double t0 = dtime();
    // shared - переменные A, B, C, N - общие для каждого потока
    // private - переменные i, j, k - локальные для каждого потока
    // omp for сам распределит итерации цикла между потоками
    // барьерная синхронизация в for стоит по умолчанию
    #pragma omp parallel for shared(A, B, C, N) private(i, j, k)
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            for (k = 0; k < N; k++) {
                C[i + j * N] += A[i + k * N] * B[k + j * N];
            }
        }
    }

    printf("time for multiplying: %f\n", dtime()-t0);
    return 0;
}
