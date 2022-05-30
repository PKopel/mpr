#!/usr/bin/env bash


for s in 1 5 10; do
    for i in {1..2}; do 
        { time ./map-reduce ${s}G-input/gutenberg-${s}G-utf-8.txt; } 2>> times-${s}G.txt
    done
done

for s in 1 5 10; do
    hdfs dfs -mkdir ${s}G-input
    hdfs dfs -put ${s}G-input/gutenberg-${s}G-utf-8.txt ${s}G-input
    for i in {1..2}; do 
        { time hadoop jar /usr/lib/hadoop/hadoop-streaming.jar \
         -files mapper.py,reducer.py \
         -mapper mapper.py \
         -reducer reducer.py \
         -input ${s}G-input \
         -output books-output; } 2>> times-${s}G-h.txt
        hdfs dfs -rm -r books-output
    done
done