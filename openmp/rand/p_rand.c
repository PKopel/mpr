#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// #define MY_RAND_MAX 2147483647 // 2^21-1 = 2097151 // 2^17-1 = 131071

// uint p_rand(uint current) {
//     uint next, shift = (uint)(MY_RAND_MAX / 5) * (current % 5);
//     switch (current % 5) {
//         case 1:
//             next = current * (uint)2147483629 + (uint)2147483587;
//             break;
//         case 2:
//             next = current * (uint)16807;
//             break;
//         default:
//             next = current * (uint)48271;
//             break;
//     }
//     return (next + shift) & MY_RAND_MAX >> 16;
// }

// #define A 742938285
// #define AHI (A >> 15)
// #define ALO (A & 0x7FFF)

// uint p_rand(uint x) {
//     uint xhi, xlo, mid;
//     xhi = x >> 16;
//     xlo = x & 0xFFFF;
//     mid = AHI * xlo + (ALO << 1) * xhi;
//     x = AHI * xhi + (mid >> 16) + ALO * xlo;
//     if (x & 0x80000000)
//         x -= 0x7FFFFFFF;
//     x += ((mid & 0xFFFF) << 15);
//     if (x & 0x80000000)
//         x -= 0x7FFFFFFF;
//     return x % MY_RAND_MAX;
// }

uint p_rand(uint current) {
    uint next = current * (uint)48271;
    return (uint)next % 524288;
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

    if (!debug) {
        printf("%d,%f,%d\n", threads, time, array[0]);
    } else {
        for (int i = 0; i < size; i++) {
            printf("%d\n", array[i]);
        }
    }

    free((void*)array);
    return 0;
}