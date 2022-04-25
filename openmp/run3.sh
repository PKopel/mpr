#!/usr/bin/env bash

echo "size,n_threads,n_buckets,fill_time,split_time,merge_b_time,sort_time,merge_a_time,time_total,sorted" > results_sort3_1.csv

for i in 1; do
    for b in 10 20 30 40 50 100 150 200 250 300; do
        for s in 1000000 5000000; do
            for _ in {1..10}; do
                echo -n "$s," >> results_sort3_1.csv
                make run-sort3 ARGS="$i $b $s" >> results_sort3_1.csv
            done
        done
    done
done

echo "size,n_threads,n_buckets,fill_time,split_time,merge_b_time,sort_time,merge_a_time,time_total,sorted" > results_sort3_2.csv

for i in {1..10}; do
    for s in 1000000 5000000; do
        for _ in {1..10}; do
            echo -n "$s," >> results_sort3_2.csv
            make run-sort3 ARGS="$i 50 $s" >> results_sort3_2.csv
        done
    done
done
