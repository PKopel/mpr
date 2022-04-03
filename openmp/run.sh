#!/usr/bin/env bash

echo "n_threads,time,rand"> results

for i in {1..10}; do
    for _ in {1..10}; do
        make run-p_rand ARGS="$i $1" >> results
    done
done