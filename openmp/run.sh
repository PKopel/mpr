#!/usr/bin/env bash

echo > results

for i in {1..5}; do
    make run-p_rand ARGS="$i $1" >> results
done