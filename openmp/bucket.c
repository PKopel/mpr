#include "bucket.h"
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
            array[i] = seed % 20;
        }
    }
    end = omp_get_wtime();

    return end - start;
}

typedef struct bucket {
    uint* array;
    int size;
    int cap;
} bucket;

bucket new_bucket(int size) {
    uint* array = (uint*)malloc(sizeof(uint) * size);
    bucket new = {array, 0, size};
    return new;
}

void insert(bucket* b, uint v) {
    b->array[b->cap] = v;
    b->cap += 1;
}
