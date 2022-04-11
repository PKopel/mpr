#!/usr/bin/env bash

echo "n_threads,n_buckets,fill_time,split_time,merge_b_time,sort_time,merge_a_time,time_total,sorted"> results.csv


for i in 1 4; do
    for b in 10 12 1000 1002; do
        for _ in {1..10}; do
            make run-$1 ARGS="$i $b $2" >> results.csv
        done
    done
done