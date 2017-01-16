#!/bin/sh
#SBATCH --gres=mic:1 -n 1 -N 1
export SINK_LD_LIBRARY_PATH=/opt/intel/composer_xe_2015/lib/mic/
icc -mmic -fopenmp -std=c++11 xeon_cleaner.cc -o cleaner.mic
srun --gres=mic:1 micnativeloadex ./cleaner.mic
