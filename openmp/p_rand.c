#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

void insert_sort(uint* array, long size){{
    int i;
    for(i = 1; i < size; i++) {
        int j = i;
        while(j > 0 && array[j-1] > array[j])
            uint t = array[j];
            array[j] = array[j-1];
            array[j-1] = t;
            j--;
        }
    }
}

int maxnum = 32768;

uint p_rand(uint current) {
    uint next = (current + 1) * 1103515243 + 12345;
    return (uint)(next / 65536) % maxnum;
}

double fill_array(uint* array, long size, int threads) {
    uint seed;
    double start, end;
    long i;
    int my_n;

    start = omp_get_wtime();
#pragma omp parallel num_threads(threads) private(i, seed, my_n) shared(array)
    {
        my_n = omp_get_thread_num();
        seed = (uint)omp_get_wtime() + (uint)my_n;
#pragma omp for schedule(dynamic, 25000000)
        for (i = 0; i < size; i++) {
            seed = p_rand(seed);
            array[i] = seed % 20;
        }
    }
    end = omp_get_wtime();

    return end - start;
}

double divide_to_buckets(uint* array, long size, Bucket* buckets, uint bucketAmount, omp_lock_t* bucketLocks) {
    long bucketSize = maxnum / bucketAmount;
    long i;
    #pragma omp parallel num_threads(threads) private(i) shared(array, buckets)
    {
        #pragma omp for schedule(dynamic, 25000000)
        for (int i = 0; i < size; i++) {
            uint value = array[i];
            uint bucketNum = size / bucketSize;
            omp_set_lock(&(bucketLocks[bucketNum]));
            insert(buckets[bucketNum], value);
            omp_unset_lock(&(bucketLocks[bucketNum]));
        }
    }
}

omp_lock_t* init_locks(long amount) {
    omp_lock_t* bucketLocks = malloc(sizeof(omp_lock_t) * amount)
    int i;
    #pragma omp parallel for
    for (i = 0; i < x; i++) {
        omp_init_lock(&(bucketLocks[i]));
    }
    return bucketLocks;
}

void destroy_locks(omp_lock_t bucketLocks) {
    int i;
    #pragma omp parallel for
    for (i = 0; i < x; i++) {
        omp_destroy_lock(&(bucketLocks[i]));
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

int main(int argc, char** argv) {
    if (argc < 3) {
        return 1;
    }

    int threads = atoi(argv[1]);
    int size = atoi(argv[2]);

    bool debug = (argc == 4) && (argv[3][0] == 'd');

    uint* array = (uint*)malloc(sizeof(uint) * size);

    double time = fill_array(array, size, threads);

    uint* array = (uint*)malloc((long)(sizeof(uint)) * size);
    if(array == NULL) {
      printf("Could not malloc\n");
      return 1;
    }

    double time = fill_array(array, size, threads);

    if (!debug) {
        printf("%f\n", time);
    } else {
        int i;
        for (i = 0; i < size; i++) {
            printf("%d\n", array[i]);
        }
    }

    free((void*)array);
    return 0;
}