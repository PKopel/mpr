#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

uint p_rand(uint current) {
    uint next = (current + 1) * 1103515243 + 12345;
    return (uint)(next / 65536) % 32768;
}

double fill_array(uint* array, int size, int threads) {
    uint seed;
    double start, end;
    int i, my_n;

    start = omp_get_wtime();
#pragma omp parallel num_threads(threads) private(i, seed, my_n) shared(array)
    {
        my_n = omp_get_thread_num();
        seed = (uint)omp_get_wtime() + (uint)my_n;
#pragma omp for schedule(static)
        for (i = 0; i < size; i++) {
            seed = p_rand(seed);
            array[i] = seed;
        }
    }
    end = omp_get_wtime();

    return end - start;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        return 1;
    }

    int threads = atoi(argv[1]);
    int size = atoi(argv[2]);

    bool debug = (argc == 4) && (argv[3][0] == 'd');

    uint* array = (uint*)malloc(sizeof(uint) * size);

    double time = fill_array(array, size, threads);

    start = omp_get_wtime();
#pragma omp parallel num_threads(threads) private(i, seed, my_n) shared(array)
    {
        seed = (uint)omp_get_wtime();
        my_n = omp_get_thread_num();
#pragma omp for schedule(static)
        for (i = my_n; i < size; i += threads) {
            seed = p_rand(seed);
            array[i] = seed;
        }
    }

    free((void*)array);
    return 0;
}