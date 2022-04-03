#!/usr/bin/env python3
import random
import sys
from mpi4py import MPI

random.seed()

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
n_threads = comm.Get_size()

comm.Barrier()

n_points = int(float(sys.argv[1]))

def check_point(x,y):
    return x ** 2 + y ** 2 <= 1


def get_point():
    return random.uniform(0, 1)


points_per_thread = n_points // n_threads
n_ok_points = 0

start = MPI.Wtime()

for _ in range(points_per_thread):
    x = get_point()
    y = get_point()
    if check_point(x,y):
        n_ok_points += 1

n_ok_points = comm.gather(n_ok_points, root=0)

end = MPI.Wtime()

if rank == 0:
    pi = 4 * sum(n_ok_points)/n_points
    time = end - start
    print(f'{n_points},{n_threads},{pi},{time}')