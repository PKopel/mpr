#!/usr/bin/env bash

echo "n_threads,fill_time,split_time,merge_b_time,sort_time,merge_a_time,sorted"> results.csv


for i in 1 4; do
    for _ in {1..10}; do
        make run-$1 ARGS="$i $2" >> results.csv
    done
done