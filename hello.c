#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

uint p_rand(uint current) {
    uint next = (current + 1) * 1103515243 + 12345;
    return (uint)(next / 65536) % 32768;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        return 1;
    }

    int threads = atoi(argv[1]);
    int size = atoi(argv[2]);
    uint* array = (uint*)malloc(sizeof(uint) * size);
    double start, end;
    int i, my_n, seed;

    //     start = omp_get_wtime();
    // #pragma omp parallel for schedule(static) num_threads(5) private(i)
    //     for (i = 0; i < size; i++) {
    //         int n_threads = omp_get_thread_num();
    //         // printf("%d\t%d\n", i, n_threads);
    //     }
    //     end = omp_get_wtime();
    //     printf("%f\n", end - start);

    start = omp_get_wtime();
#pragma omp parallel num_threads(threads) private(i) private(seed) private(my_n) shared(array)
    {
        seed = omp_get_thread_num();
        my_n = seed;
#pragma omp for schedule(static)
        for (i = my_n; i < size; i += threads) {
            seed = p_rand(seed);
            array[i] = seed;
        }
    }
    end = omp_get_wtime();
    // printf("%f\t%d\n", end - start, array[0]);

    for (i = 0; i < size; i++) {
        printf("%d\n", array[i]);
    }

    free((void*)array);
    return 0;
}