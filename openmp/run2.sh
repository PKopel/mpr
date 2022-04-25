#!/usr/bin/env bash

echo "size,n_threads,n_buckets,fill_time,split_time,sort_time,array_time,time_total,sorted" > results_sort2_1.csv

for i in 1; do
    for b in 1 2 3 4 5 10 15 20 25 30; do
        for s in 1000000 5000000; do
            for _ in {1..10}; do
                bnum=$((s / 100 * b ))
                bsize=$((bnum * 30))
                echo -n "$s," >> results_sort2_1.csv
                make run-sort2 ARGS="$i $s $bnum $bsize" BUCKET="">> results_sort2_1.csv
            done
        done
    done
done

# echo "size,n_threads,n_buckets,fill_time,split_time,sort_time,array_time,time_total,sorted" > results_sort2_2.csv

# for i in {1..10}; do
#     for s in 1000000 5000000; do
#         for _ in {1..10}; do
#             echo -n "$s," >> results_sort2_2.csv
#             make run-sort2 ARGS="$i 50 $s" >> results_sort2_2.csv
#         done
#     done
# done
