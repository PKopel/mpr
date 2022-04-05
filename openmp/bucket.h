#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct bucket {
    uint* array;
    int size;
    int cap;
} bucket;

bucket new_bucket(int size);

double fill_array(uint* array, int size, int threads);

uint p_rand(uint current);