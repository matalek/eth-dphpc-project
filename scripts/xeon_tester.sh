#!/bin/sh
#SBATCH --gres=mic:1 -n 1 -N 1
export SINK_LD_LIBRARY_PATH=/opt/intel/composer_xe_2015/lib/mic/
export OMP_NUM_THREADS=64
icc -mmic -fopenmp -O3 -std=c++11 xeon_tester.cc -o tester.mic
srun --gres=mic:1 micnativeloadex ./tester.mic
