#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MY_RAND_MAX 32768
#define N_BUCKETS 10

typedef struct bucket {
    uint* array;
    int size;
    int cap;
} bucket;

uint p_rand(uint current) {
    uint next = (current + 1) * 1103515243 + 12345;
    return (uint)(next / 65536) % MY_RAND_MAX;
}

bucket new_bucket(int size) {
    uint* array = (uint*)malloc(sizeof(uint) * size);
    bucket new = {array, 0, size};
    return new;
}

void insert(bucket* b, uint v) {
    b->array[b->size] = v;
    b->size += 1;
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

double split_buckets(uint* array, int size, bucket** buckets, int threads) {
    uint current;
    int i, bi, my_n;
    double start, end;

    start = omp_get_wtime();
#pragma omp parallel num_threads(threads) private(i, bi, my_n, current) shared(array, buckets)
    {
        my_n = omp_get_thread_num();
#pragma omp for schedule(static)
        for (i = 0; i < size; i++) {
            current = array[i];
            bi = N_BUCKETS * current / MY_RAND_MAX;
            insert(&buckets[my_n][bi], current);
        }
    }
    end = omp_get_wtime();

    return end - start;
}

double sort_buckets(bucket** buckets, int threads) {
    bucket b;
    double start, end;

    start = omp_get_wtime();
    for (int i = 0; i < N_BUCKETS; i++) {
        b = buckets[0][i];
        // insert_sort(&b.array, b.size);
    }
    end = omp_get_wtime();

    return end - start;
}

double merge_buckets(uint* array, bucket** buckets, int threads) {
    double start, end;

    start = omp_get_wtime();

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
    bucket** buckets = (bucket**)malloc(sizeof(bucket*) * threads);
    for (int j = 0; j < threads; j++) {
        buckets[j] = (bucket*)malloc(sizeof(bucket) * N_BUCKETS);
        for (int i = 0; i < N_BUCKETS; i++) {
            buckets[j][i] = new_bucket(size * 0.3);
        }
    }

    double time_fill = fill_array(array, size, threads);
    double time_split = split_buckets(array, size, buckets, threads);
    double time_sort = sort_buckets(buckets, threads);
    double time_merge = merge_buckets(array, buckets, threads);

    if (!debug) {
        printf("%d,%f,%f\n", threads, time_fill, time_split);
    } else {
        for (int i = 0; i < size; i++) {
            printf("%d\n", array[i]);
        }
    }

    free((void*)array);
    return 0;
}