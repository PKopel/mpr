#!/usr/bin/env bash

echo "n_threads,n_buckets,fill_time,split_time,merge_b_time,sort_time,merge_a_time,time_total,sorted"> results.csv


for i in 1 4; do
    for b in 10 100 1000 10000 100000; do
        for s in 1000 10000 100000 1000000; do
            make run-$1 ARGS="$i $b $s" >> results.csv
        done
    done
done