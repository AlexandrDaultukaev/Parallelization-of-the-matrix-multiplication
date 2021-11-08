#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <sys/time.h>
#include <omp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include "CLI11.hpp"

double *alloc_matrix(int size){
    double *matrix = (double*)malloc(size * size * sizeof(double));
    //printf("matrix %dx%d allocated\n", size, size);
    return matrix;
}

void del_matrix(double *matrix){
    free(matrix);
}

double dtime(){
    struct timeval tv;
    struct timezone tz;
    double t;
    gettimeofday(&tv, &tz);
    t = (double)tv.tv_sec + (double)tv.tv_usec*1e-6;
    return(t);
}

struct matrix_args{
    double *m1;
    double *m2;
    double *ans;
    int size;
    int start;
    int end;
} ARG[100];

void *multiply_matrix(void *pargs){
    struct matrix_args *args = (struct matrix_args *) pargs;
    int i, j, k, l, m, tmp = 0;
    double t0 = dtime();
    double *m1 = args->m1;
    double *m2 = args->m2;
    double *ans = args->ans;
    int size = args->size;
    int start = args->start;
    int end = args->end;
    for(i = start; i < end; i++){
        m = i * size;
        for(j = 0; j < size; j++){
            l = 0;
            for(k = 0; k < size; k++){
                tmp += m1[i * size + k] * m2[j + l];
                l += size;
            }
            ans[m + j] = tmp;
            tmp = 0;
        }
    }
    //printf("thread works %fs\n", dtime() - t0);
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    int i, j, size, k, step, pos, res;
    int THR_NUM,THR,N;

    CLI::App app{"App description"};
    app.add_option("-n", N, "Size");
    app.add_option("-t", THR, "Number of threads");
    CLI11_PARSE(app, argc, argv);

    THR_NUM = THR;
    size = N;

    /* Создаем массив потоков */
    pthread_t threads[THR_NUM];
    pthread_attr_t attr;
    //printf("allocating\n");
    double *m1 = alloc_matrix(size);
    double *m2 = alloc_matrix(size);
    double *ans = alloc_matrix(size);
    srand(time(NULL));

    for(int i=0; i<size; i++) {
        for(int j=0; j<size; j++){
            m1[i + j * size] = rand() % 5;
            m2[i + j * size] = rand() % 5;
        }
    }

    //printf("multiplying\n");
    double t0 = dtime();
    /* Инициализация атрибутов потока */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    step = (int)((double)size/(double)THR_NUM);
    pos = 0;
    for(k = 0; k < THR_NUM; k++) {
        ARG[k].m1 = m1;
        ARG[k].m2 = m2;
        ARG[k].ans = ans;
        ARG[k].size = size;
        ARG[k].start = pos;
        pos += step;
        ARG[k].end = (k == THR_NUM - 1) ? size : pos;
        res = pthread_create(&threads[k], &attr, multiply_matrix, (void *)&ARG[k]);
        if(res){
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }
    pthread_attr_destroy(&attr);
    for(k = 0; k < THR_NUM; k++)
        pthread_join(threads[k], NULL);
    printf("time for multiplying (n = %d, t = %d): %f\n", size, THR_NUM, dtime()-t0);
    del_matrix(m1);
    del_matrix(m2);
    del_matrix(ans);
    pthread_exit(NULL);
    return 0;
}
