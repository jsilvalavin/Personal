#!/bin/bash

#SBATCH --partition=full

#SBATCH --job-name=IMT2112

#SBATCH --output=log.out

#SBATCH --ntasks=30
#SBATCH --cpus-per-task=1

mpic++ code.cpp -std=c++11
time mpirun -np 1 ./a.out 40000
time mpirun -np 5 ./a.out 40000
time mpirun -np 10 ./a.out 40000
time mpirun -np 15 ./a.out 40000
time mpirun -np 20 ./a.out 40000
time mpirun -np 25 ./a.out 40000
time mpirun -np 30 ./a.out 40000