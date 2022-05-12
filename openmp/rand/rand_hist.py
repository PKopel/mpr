#! /usr/bin/env python3
import matplotlib.pyplot as plt

with open('rand.csv') as file:
    lines = file.readlines()

lines = list(map(int, lines))

fig = plt.figure(figsize=(30,10))
max_val = max(lines)
min_val = min(lines)
plt.hist(lines, bins=10000)
plt.savefig('rand_hist.png')
