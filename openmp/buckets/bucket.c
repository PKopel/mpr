#include "bucket.h"
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

uint p_rand(uint current) {
    uint next = current * (uint)48271;
    return (uint)next % MY_RAND_MAX;
}

bucket new_bucket(int cap) {
    uint* array = (uint*)malloc(sizeof(uint) * cap);
    bucket new = {array, 0, cap};
    return new;
}

void insert_sort(uint* array, int size) {
    int i;
    uint t;
    for (i = 1; i < size; i++) {
        int j = i;
        while (j > 0 && array[j - 1] > array[j]) {
            t = array[j];
            array[j] = array[j - 1];
            array[j - 1] = t;
            j--;
        }
    }
}
