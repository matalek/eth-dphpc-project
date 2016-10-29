#!/usr/bin/env python
import subprocess
import csv
import sys
import numpy as np
import matplotlib.pyplot as plt

# Usage ./sequentialScript.py -f <path to c++ program> -g <path to points generator program> -c
# <Number of different combinations of number of points> -w <width of steps> -s <starting number of points>
# -r <number of repetition for each number of points>

if int(sys.argv[12]) <= 0:
    print 'ERROR in usage, provide int number of repetition for each value (ex. -r 20)'
    exit
if int(sys.argv[10]) <= 0:
    print 'ERROR in usage, provide int starting number of points (ex. -s 100)'
    exit
if int(sys.argv[8]) <= 0:
    print 'ERROR in usage, provide int width in steps (ex. -w 50)'
    exit
if int(sys.argv[6]) <= 0:
    print 'ERROR in usage, provide int number of different combinations (ex. -c 20)'
    exit


rep_number = int(sys.argv[12])
starting_value = int(sys.argv[10])
step_width = int(sys.argv[8])
comb_number = int(sys.argv[6])

cpp_file_path = sys.argv[2]
generator_path = sys.argv[4]

subprocess.call('g++ -std=c++11 ' + cpp_file_path + ' -o test', shell=True)
subprocess.call('g++ -std=c++11 ' + generator_path + ' -o generator', shell=True)
subprocess.call('mkdir -p logs', shell=True)

exec_time = []

ofile  = open('log_results.csv', "wb")
writer = csv.writer(ofile, delimiter='	', quotechar='"', quoting=csv.QUOTE_ALL)
columns_indexes = ['#Points','Exec_Time [us]']
writer.writerow(columns_indexes)

for comb in range(0 , comb_number):
    time_accumulator = 0
    for take in range (0 , rep_number):
        num_of_points = starting_value + step_width*comb
        result = subprocess.check_output('./generator -p ' + str(num_of_points) +
                                         ' | ./test | tee logs/log' + str(comb+1) + 'take' + str(take+1) + '.log', shell=True)
        curr_time = int(result.split('\n')[0].split(' ')[1])
        time_accumulator += curr_time
    avg_time = time_accumulator/rep_number
    exec_time.append(avg_time)
    writer.writerow([num_of_points,avg_time])

ofile.close()
x = np.arange(starting_value, (starting_value + step_width*comb_number) , step_width)
y = np.array(exec_time)


plt.title("Number of input points")
plt.plot(x, y, 'b-', linewidth = 2.0)
plt.ylabel('Execution time [us]')
plt.xlabel('Number of input points')
plt.grid(True)
plt.show()