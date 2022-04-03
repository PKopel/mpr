#!/bin/bash -l
#SBATCH --nodes 1
#SBATCH --ntasks 12
#SBATCH --time=01:00:00
#SBATCH --partition=plgrid-short
#SBATCH --account=plgmpr22
#SBATCH --sockets-per-node=2

echo "size,n_threads,pi,time" > results_${1}.csv

module add plgrid/tools/openmpi
module load plgrid/libs/python-mpi4py/3.0.1-python-3.6


for I in {1..10}; do
    for THREADS in {1..12}; do
        mpiexec -np $THREADS ./pi_monte.py $1 >> results_${1}.csv
    done
done