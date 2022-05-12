#!/usr/bin/env bash

# echo "n_buckets,size,n_threads,fill_time,split_time,sort_time,array_time,time_total" > results_sort2_1.csv

# for i in 1; do
#     for b in 1 2 3 4 5 10 15 20 25 30; do
#         for s in 5000000 1000000; do
#             for _ in {1..5}; do
#                 bnum=$((s / 100 * b ))
#                 make run-sort2 ARGS="$i $s $bnum $s" BUCKET="">> results_sort2_1.csv
#             done
#         done
#     done
# done

echo "n_buckets,size,n_threads,fill_time,split_time,sort_time,array_time,time_total,sorted" > results_sort2_2.csv

for i in {1..6}; do
    for s in 1000000; do
        for _ in {1..10}; do
            make run-sort2 ARGS="$i $s 750000 500000" BUCKET="">> results_sort2_2.csv
        done
    done
done
