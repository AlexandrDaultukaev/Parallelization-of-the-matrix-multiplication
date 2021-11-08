#!/bin/bash

echo "Compiling..."

make

echo "Collecting openMP data"

for ((i=1000; i<=5000;i+=2000)) do
    for ((j=1; j<=4; j++)) do
        ./benchmark_omp -n $i -t $j
    done
done

echo "Collecting Pthreads data"

for ((i=1000; i<=5000;i+=2000)) do
    for ((j=1; j<=4; j++)) do
        ./benchmark_pthreads -n $i -t $j
    done
done

echo "Done!"
