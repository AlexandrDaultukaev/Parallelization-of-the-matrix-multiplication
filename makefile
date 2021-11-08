all: benchmark_omp benchmark_pthread

benchmark_omp: multi_omp.c
	g++ -Wall -o benchmark_omp multi_omp.c -fopenmp

benchmark_pthread: multi_pthread.c
	g++ -Wall -o benchmark_pthread multi_pthread.c -fopenmp

