#!/usr/bin/env python
import subprocess
import csv
import sys
import numpy as np
import matplotlib.pyplot as plt

# Usage ./algorithm_comparator.py
# -c <number of different combinations of number of points> 
# -w <width of steps>
# -s <starting number of points>
# -r <range of points coordinates>
# -R <number of repetition for each number of points>
# -a <algorithms to compare in the format <algo1:num_of_threads algo2:num_of_threads algo3:num_of_threads ...>>

if int(sys.argv[10]) <= 0 or len(sys.argv) <= 11:
    print 'ERROR in usage, provide int number of repetition for each value (ex. -R 20)'
    exit
if int(sys.argv[8]) <= 0 or len(sys.argv) <= 9:
    print 'ERROR in usage, provide int range of points coordinates (ex. -r 20)'
    exit   
if int(sys.argv[6]) <= 0 or len(sys.argv) <= 7:
    print 'ERROR in usage, provide int starting number of points (ex. -s 100)'
    exit
if int(sys.argv[4]) <= 0 or len(sys.argv) <= 5:
    print 'ERROR in usage, provide int width in steps (ex. -w 50)'
    exit
if int(sys.argv[2]) <= 0 or len(sys.argv) <= 3:
    print 'ERROR in usage, provide int number of different combinations (ex. -c 20)'
    exit

args = " ".join(sys.argv[1:11])

rep_number = int(sys.argv[10])
coordinates_range = int(sys.argv[8])
starting_value = int(sys.argv[6])
step_width = int(sys.argv[4])
comb_number = int(sys.argv[2])

subprocess.call('./file_logger.py -a sequential/convex_hull_graham ' + args, shell=True)

for value in range(0,len(sys.argv) - 12):
    algorithm = sys.argv[12 + value].split(":")
    subprocess.call('./file_logger.py -a ' + algorithm[0] + ' ' + args + ' -t '
                    + algorithm[1], shell=True)

subprocess.call('./plotter.py -c ' + str(comb_number) + ' -w ' + str(step_width) + ' -s ' + str(starting_value) +
                ' -a ' + (" ".join(sys.argv[12:])) + ' sequential/convex_hull_graham', shell=True)
