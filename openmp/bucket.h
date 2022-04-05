#include <omp.h>
#include <stdlib.h>

#define MY_RAND_MAX 32768

typedef struct bucket {
    uint* array;
    int size;
    int cap;
} bucket;

bucket new_bucket(int size);

uint p_rand(uint current);

void insert_sort(uint* array, int size);