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

# Comparator to ensure correctness of result
def compare_function(cgal , alg):
    # create array with points (remove last char which is \n)
    alg_points = alg[:-1].split('\n')[2:]
    cgal_points = cgal[:-1].split('\n')[2:]

    for point in cgal_points:
        if point not in alg_points:
            return False
    for point in alg_points:
        if point not in cgal_points:
            return False
    return True


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

CONST_REP_NUMBER = int(sys.argv[10])
CONST_COORDINATE_RANGE = int(sys.argv[8])
CONST_STARTING_VALUE = int(sys.argv[6])
CONST_STEP_WIDTH = int(sys.argv[4])
CONST_COMB_NUMBER = int(sys.argv[2])

key_value = {}

#build executables
subprocess.call('make clean', shell=True)
subprocess.call('make', shell=True)
subprocess.call('make -C generator/', shell=True)
subprocess.call('mkdir -p log_files', shell=True)
subprocess.call('(cd cgal && cmake .)', shell=True)
subprocess.call('(cd cgal && make)', shell=True)

#Generate CSVs for eveery algorithm tested
for value in range(0, len(sys.argv) - 12):
    algorithm = sys.argv[12 + value]
    # create and open a csv file to store results
    ofile = open('log_files/log_results_' + algorithm.replace('/', '_').replace(':','_t_') + '.csv', "wb")
    writer = csv.writer(ofile)
    writer.writerow(['#Input Points', 'Exec_Time [us]'])
    ofile.close()
    key_value[algorithm] = 0

for num_of_points in range(
        CONST_STARTING_VALUE, CONST_STEP_WIDTH*CONST_COMB_NUMBER + CONST_STARTING_VALUE, CONST_STEP_WIDTH):

    print ('\n-----------------------------------------\nBEGINNING STEP: ' +
           str(((num_of_points-CONST_STARTING_VALUE)/CONST_STEP_WIDTH)+1) +
           ', POINTS:' + str(num_of_points) + '\n-----------------------------------------'
           )

    for repetition in range(0, CONST_REP_NUMBER):
        print '\nGenerationg points for TAKE ' + str(repetition) + '...\n'

        subprocess.call(
            'echo "' + str(num_of_points) + ' ' + str(CONST_COORDINATE_RANGE)
            + '" | ./generator/generator > tmp.log', shell=True)

        # Apply CGAL_algorithm to compare our result to
        cgal_result = subprocess.check_output(
            'cat tmp.log | ./cgal/cgal_graham_andrew', shell=True)

        for value in range(0,len(sys.argv) - 12):
            algorithm = sys.argv[12 + value]
            algorithm_name = algorithm.split(":")[0]
            concurrency = algorithm.split(":")[1]

            print(algorithm + ' REP ' + str(repetition + 1))

            # Apply given algorithm. Output of algorithm: time\n resulting_points
            alg_result = subprocess.check_output(
                'cat tmp.log | ./tester ' + algorithm_name + ' ' + concurrency, shell=True)

            # evaluate correctness on points array
            if not compare_function(cgal_result, alg_result):
                print 'ERROR, ALGORITHM INCORRECT'
                print 'AGLORITHM:\n' + alg_result
                print 'CGAL:\n' + cgal_result
                sys.exit()

            key_value[algorithm] += int(alg_result.split('\n')[0].split(' ')[1])

    for value in key_value:
        algorithm = value
        ofile = open('log_files/log_results_' + algorithm.replace('/', '_').replace(':', '_t_') + '.csv', "a")
        writer = csv.writer(ofile)
        writer.writerow([num_of_points, key_value[value]/CONST_REP_NUMBER])
        ofile.close()

print('\n--------------------------------------\n| ----------------------------------- |\n'
      '| |COMPARISON COMPLETED SUCCESSFULLY| |'
      '\n| ----------------------------------- |\n--------------------------------------\n')
# subprocess.call('./plotter.py -c ' + str(CONST_COMB_NUMBER) + ' -w ' + str(CONST_STEP_WIDTH) + ' -s ' + str(CONST_STARTING_VALUE) +
#                 ' -a ' + (" ".join(sys.argv[12:])) + ' sequential/convex_hull_graham_coordinate_sort', shell=True)
subprocess.call('./plotter.py -c ' + str(CONST_COMB_NUMBER) + ' -w ' + str(CONST_STEP_WIDTH) + ' -s ' + str(CONST_STARTING_VALUE) +
                ' -a ' + (" ".join(sys.argv[12:])), shell=True)
