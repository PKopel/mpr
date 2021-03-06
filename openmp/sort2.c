#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct bucket {
    uint* array;
    int size;
    int cap;
} bucket;

bucket* new_bucket(int cap) {
    uint* array = (uint*)malloc(sizeof(uint) * cap);
    if (array == NULL) {
        printf("Could not malloc bucket's array\n");
        exit(1);
    }
    bucket* new = (bucket*)malloc(sizeof(bucket));
    if (new == NULL) {
        printf("Could not malloc bucket\n");
        exit(1);
    }
    new->array = array;
    new->size = 0;
    new->cap = cap;
    return new;
}

void insert(bucket* b, uint v) {
    b->array[b->size] = v;
    b->size += 1;
}

bucket** create_buckets(int amount, int bucketSize, int threads) {
    bucket** buckets = (bucket**)malloc(sizeof(bucket*) * amount);
    if (buckets == NULL) {
        printf("Could not malloc buckets\n");
        exit(1);
    }
    int i = 0;
#pragma omp parallel for num_threads(threads)
    for (i = 0; i < amount; i++) {
        buckets[i] = new_bucket(bucketSize);
    }
    return buckets;
}

void insert_sort(uint* array, int arraySize) {
    int i;
    for (i = 1; i < arraySize; i++) {
        int j = i;
        while (j > 0 && array[j - 1] > array[j]) {
            uint t = array[j];
            array[j] = array[j - 1];
            array[j - 1] = t;
            j--;
        }
    }
}

int maxnum = 2097151;

uint p_rand(uint current) {
    uint next = current * (uint)48271;
    return (uint)next % maxnum;
}

void fill_array(uint* array, int size, int threads) {
    uint seed;
    int i;
    int my_n;

#pragma omp parallel num_threads(threads) private(i, seed, my_n) shared(array)
    {
        my_n = omp_get_thread_num();
        seed = (uint)omp_get_wtime() + (uint)my_n;

#pragma omp for schedule(dynamic, 1000)
        for (i = 0; i < size; i++) {
            seed = p_rand(seed + i);
            array[i] = seed;
        }
    }
}

double divide_to_buckets(uint* array, int size, bucket** buckets, int bucketAmount, omp_lock_t* bucketLocks, int threads) {
    int i;
#pragma omp parallel num_threads(threads) private(i) shared(array, buckets)
    {
#pragma omp for schedule(dynamic, 1000)
        for (i = 0; i < size; i++) {
            uint value = array[i];
            uint bucketNum = bucketAmount * value / maxnum;
            omp_set_lock(&(bucketLocks[bucketNum]));
            insert(buckets[bucketNum], value);
            omp_unset_lock(&(bucketLocks[bucketNum]));
        }
    }
}

void print_buckets(bucket** buckets, uint bucketAmount) {
    int i, j;
    for (i = 0; i < bucketAmount; i++) {
        printf("Bucket %d: ", i);
        for (j = 0; j < buckets[i]->size; j++) {
            printf("%d,", buckets[i]->array[j]);
        }
        printf("\n");
    }
}

omp_lock_t* init_locks(int amount, int threads) {
    omp_lock_t* bucketLocks = malloc(sizeof(omp_lock_t) * amount);
    int i;
#pragma omp parallel for num_threads(threads)
    for (i = 0; i < amount; i++) {
        omp_init_lock(&(bucketLocks[i]));
    }
    return bucketLocks;
}

void destroy_locks(omp_lock_t* bucketLocks, uint bucketAmount, int threads) {
    int i;
#pragma omp parallel for num_threads(threads) private(i) shared(bucketLocks, bucketAmount)
    for (i = 0; i < bucketAmount; i++) {
        omp_destroy_lock(&(bucketLocks[i]));
    }
}

void destroy_buckets(bucket** buckets, uint bucketAmount, int threads) {
    int i;
#pragma omp parallel for num_threads(threads)
    for (i = 0; i < bucketAmount; i++) {
        free(buckets[i]->array);
        free(buckets[i]);
    }
    free(buckets);
}

void sort_buckets(bucket** buckets, uint bucketAmount, int threads) {
    int i;
#pragma omp parallel num_threads(threads) private(i) shared(buckets, bucketAmount)
    {
#pragma omp for schedule(dynamic, 1000)
        for (i = 0; i < bucketAmount; i++) {
            insert_sort(buckets[i]->array, buckets[i]->size);
        }
    }
}

void copy_to_array(bucket** buckets, uint bucketAmount, uint* array, int threads) {
    int* sums = (int*)malloc(sizeof(int) * (bucketAmount + 1));
    int i;
    sums[0] = 0;
    for (i = 0; i < bucketAmount; i++) {
        sums[i + 1] = sums[i] + buckets[i]->size;
    }
#pragma omp parallel private(i) shared(buckets, bucketAmount, array) num_threads(threads)
    {
#pragma omp for schedule(dynamic, 1000)
        for (i = 0; i < bucketAmount; i++) {
            memcpy(array + sums[i], buckets[i]->array, buckets[i]->size * sizeof(uint));
        }
    }
    free(sums);
}

bool is_sorted(uint* array, uint length) {
    int i;
    int prev = -1;
    for (i = 0; i < length; i++) {
        if (prev > (int)array[i])
            return false;
        prev = array[i];
    }
    return true;
}

int main(int argc, char** argv) {
    if (argc < 5) {
        printf("args: threads array_size bucket_amount bucket_size\n");
        return 1;
    }

    int threads = atoi(argv[1]);
    int size = atol(argv[2]);
    int bucketAmount = atol(argv[3]);
    int bucketSize = atol(argv[4]);

    uint* array = (uint*)malloc((int)(sizeof(uint)) * size);
    if (array == NULL) {
        printf("Could not malloc array\n");
        exit(1);
    }

    // double bucketCreationStart;
    // double lockCreationStart;
    double generationStart;
    double bucketDivisionStart;
    double bucketSortStart;
    double bucketCopyStart;
    double algEnd;

    // bucketCreationStart = omp_get_wtime();
    bucket** buckets = create_buckets(bucketAmount, bucketSize, threads);

    // lockCreationStart = omp_get_wtime();
    omp_lock_t* bucketLocks = init_locks(bucketAmount, threads);

    generationStart = omp_get_wtime();
    fill_array(array, size, threads);

    bucketDivisionStart = omp_get_wtime();
    divide_to_buckets(array, size, buckets, bucketAmount, bucketLocks, threads);

    bucketSortStart = omp_get_wtime();
    sort_buckets(buckets, bucketAmount, threads);

    bucketCopyStart = omp_get_wtime();
    copy_to_array(buckets, bucketAmount, array, threads);

    algEnd = omp_get_wtime();

    destroy_locks(bucketLocks, bucketAmount, threads);
    destroy_buckets(buckets, bucketAmount, threads);

    // print times
    // printf("Buckets: %ld\n", bucketAmount);
    // printf("Array: %ld\n", size);
    // printf("Generation time [s]:  %f\n", bucketDivisionStart - generationStart);
    // printf("Bucket division time [s]: %f\n", bucketSortStart - bucketDivisionStart);
    // printf("Bucket sort time [s]: %f\n", bucketCopyStart - bucketSortStart);
    // printf("Copy to array time [s]: %f\n", algEnd - bucketCopyStart);
    // printf("Time [s]: \n", algEnd - generationStart)
    // if (is_sorted(array, size)) {
    //     printf("Array sorted succesfully.\n");
    // } else {
    //     printf("Array sorting error. \n");
    // }
    // bucketAmount,arraySize,Threads,generationTime,bucketDivisionTime,bucketSortTime,copyToArrayTime,algTime
    printf("%d,%d,%d,%f,%f,%f,%f,%f\n", bucketAmount, size, threads, bucketDivisionStart - generationStart, bucketSortStart - bucketDivisionStart, bucketCopyStart - bucketSortStart, algEnd - bucketCopyStart, algEnd - generationStart);

    free(array);
    return 0;
}