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

void swap(uint* xp, uint* yp);

void insert_sort(uint* array, int size);

void bubble_sort(uint* array, int n);