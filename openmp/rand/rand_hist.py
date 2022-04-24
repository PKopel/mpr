#! /usr/bin/env python3
import matplotlib.pyplot as plt

with open('rand.csv') as file:
    lines = file.readlines()

fig = plt.figure(figsize=(100,10))
plt.hist(lines, bins=1000)
plt.savefig('rand_hist.png')
