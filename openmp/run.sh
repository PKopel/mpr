#!/usr/bin/env bash

echo > results

for i in {1..5}; do
    make run-p_rand ARGS="$i 100000000000" >> results
done