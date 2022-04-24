#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buckets/bucket.h"

int chunks;

double fill_array(uint* array, int size, int threads) {
    uint seed;
    double start, end;
    int i, my_n;

    start = omp_get_wtime();
#pragma omp parallel num_threads(threads) private(i, seed, my_n) shared(array)
    {
        my_n = omp_get_thread_num();
        seed = (uint)omp_get_wtime() + (uint)my_n;
#pragma omp for schedule(static, size / threads)
        for (i = 0; i < size; i++) {
            seed = p_rand(seed);
            array[i] = seed;
        }
    }
    end = omp_get_wtime();

    return end - start;
}

double split_buckets(uint* array, int size, bucket** buckets, int n_buckets, int threads) {
    uint current;
    bucket b;
    int i, bi, nb, my_n;
    double start, end;

    start = omp_get_wtime();
#pragma omp parallel num_threads(threads) private(i, bi, my_n, current, b, nb) shared(array, buckets)
    {
        my_n = omp_get_thread_num();
        nb = n_buckets;
#pragma omp for schedule(static, size / threads)
        for (i = 0; i < size; i++) {
            current = array[i];
            bi = current / ((MY_RAND_MAX / nb) + 1);
            b = buckets[my_n][bi];
            if (b.size + 1 >= b.cap) {
                printf("%d %d %d %d\n", my_n, bi, b.size, b.cap);
                fflush(NULL);
                exit(1);
            }
            b.array[b.size] = current;
            (&buckets[my_n][bi])->size += 1;
        }
    }
    end = omp_get_wtime();

    return end - start;
}

double merge_buckets(bucket** buckets, int n_buckets, int threads) {
    bucket b;
    uint* s;
    int j, i, nb, nt, sum = 0;
    double start, end;

    start = omp_get_wtime();
#pragma omp parallel num_threads(threads) private(j, i, sum, b, s, nb, nt) shared(buckets)
    {
        nb = n_buckets;
        nt = threads;
#pragma omp for schedule(static, nb / threads)
        for (i = 0; i < nb; i++) {
            sum = 0;
            for (j = 0; j < nt; j++) {
                sum += buckets[j][i].size;
            }
            b = new_bucket(sum);
            for (j = 0; j < nt; j++) {
                s = &b.array[b.size];
                memcpy(s, buckets[j][i].array, buckets[j][i].size * sizeof(uint));
                b.size += buckets[j][i].size;
                // clean up unused buckets
                free(buckets[j][i].array);
                buckets[j][i].size = 0;
            }
            buckets[0][i] = b;
        }
    }
    end = omp_get_wtime();

    return end - start;
}

double sort_buckets(bucket* buckets, int n_buckets, int threads) {
    bucket b;
    int i, nb;
    double start, end;

    start = omp_get_wtime();
#pragma omp parallel num_threads(threads) private(i, b, nb) shared(buckets)
    {
        nb = n_buckets;
#pragma omp for schedule(static, nb / threads)
        for (i = 0; i < nb; i++) {
            b = buckets[i];
            insert_sort(b.array, b.size);
        }
    }
    end = omp_get_wtime();

    return end - start;
}

double merge_array(uint* array, bucket* buckets, int n_buckets, int threads) {
    uint* s;
    int i, j, nb, size;
    double start, end;

    start = omp_get_wtime();
#pragma omp parallel num_threads(threads) private(i, j, s, nb, size) shared(array, buckets)
    {
        nb = n_buckets;
#pragma omp for schedule(static, nb / threads)
        for (i = 0; i < nb; i++) {
            if (buckets[i].size != 0) {
                size = 0;
                for (j = 0; j < i; j++) {
                    size += buckets[j].size;
                }
                s = &array[size];
                memcpy(s, buckets[i].array, buckets[i].size * sizeof(uint));
            }
        }
    }
    end = omp_get_wtime();

    return end - start;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        return 1;
    }

    int threads = atoi(argv[1]);
    int bucket_promil = atoi(argv[2]);
    int size = atoi(argv[3]);
    chunks = size / 4;
    int n_buckets = size * bucket_promil / 1000;

    bool debug = (argc == 5) && (argv[4][0] == 'd');

    uint* array = (uint*)malloc(sizeof(uint) * size);
    bucket** buckets = (bucket**)malloc(sizeof(bucket*) * threads);
    int b_size = size / n_buckets * 100;
    b_size = b_size > 0 ? b_size : 1;
    for (int j = 0; j < threads; j++) {
        buckets[j] = (bucket*)malloc(sizeof(bucket) * n_buckets);
        for (int i = 0; i < n_buckets; i++) {
            buckets[j][i] = new_bucket(b_size);
        }
    }

    double time_fill = fill_array(array, size, threads);
    double time_split = split_buckets(array, size, buckets, n_buckets, threads);
    double time_merge_b = merge_buckets(buckets, n_buckets, threads);
    double time_sort = sort_buckets(buckets[0], n_buckets, threads);
    double time_merge_a = merge_array(array, buckets[0], n_buckets, threads);
    double time_total = time_fill + time_split + time_merge_b + time_sort + time_merge_a;

    bool sorted = true;
    for (int i = 1; i < size; i++) {
        sorted = sorted && array[i - 1] <= array[i];
    }

    if (!debug) {
        printf("%d,%d,%f,%f,%f,%f,%f,%f,%d\n",
               threads,
               n_buckets,
               time_fill,
               time_split,
               time_merge_b,
               time_sort,
               time_merge_a,
               time_total,
               sorted);
    } else {
        for (int i = 0; i < size; i++) {
            printf("%d\n", array[i]);
        }
    }

    free((void*)array);
    for (int i = 0; i < n_buckets; i++) {
        free((void*)buckets[0][i].array);
    }
    for (int j = 0; j < threads; j++) {
        free((void*)buckets[j]);
    }
    free((void*)buckets);
    return 0;
}