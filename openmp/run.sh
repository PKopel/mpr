#!/usr/bin/env bash

echo "size,n_threads,n_buckets,fill_time,split_time,merge_b_time,sort_time,merge_a_time,time_total,sorted"> results_1.csv


for i in 1; do
    for b in 300 350 400; do
        for s in 100000 500000 1000000; do
            for _ in {1..10}; do
                echo -n "$s," >> results_1.csv
                make run-$1 ARGS="$i $b $s" >> results_1.csv
            done
        done
    done
done
