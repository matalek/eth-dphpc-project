#!/usr/bin/env python
import subprocess
import csv
import sys
import numpy as np
import matplotlib.pyplot as plt

# Usage ./tester_script.py -a <algorithm path> 
# -c <number of different combinations of number of points> 
# -w <width of steps> -s <starting number of points>
# -r <range of points coordinates>
# -R <number of repetition for each number of points>

if int(sys.argv[12]) <= 0:
    print 'ERROR in usage, provide int number of repetition for each value (ex. -R 20)'
    exit
if int(sys.argv[10]) <= 0:
    print 'ERROR in usage, provide int range of points coordinates (ex. -r 20)'
    exit   
if int(sys.argv[8]) <= 0:
    print 'ERROR in usage, provide int starting number of points (ex. -s 100)'
    exit
if int(sys.argv[6]) <= 0:
    print 'ERROR in usage, provide int width in steps (ex. -w 50)'
    exit
if int(sys.argv[4]) <= 0:
    print 'ERROR in usage, provide int number of different combinations (ex. -c 20)'
    exit


rep_number = int(sys.argv[12])
coordinates_range = int(sys.argv[10])
starting_value = int(sys.argv[8])
step_width = int(sys.argv[6])
comb_number = int(sys.argv[4])

algorithm = sys.argv[2]

#build executables
subprocess.call('make ALGORITHM=' + algorithm, shell=True)
subprocess.call('make -C generator/', shell=True)
subprocess.call('mkdir -p log_files', shell=True)

exec_time = []

#create and open a csv file to store results
ofile  = open('log_files/log_results_' + algorithm.replace('/', '_') + '.csv', "wb")
writer = csv.writer(ofile, delimiter='	', quotechar='"', quoting=csv.QUOTE_ALL)
writer.writerow(['#Points','Exec_Time [us]'])

#execute tests with different takes for each number of points
for comb in range(0 , comb_number):
    time_accumulator = 0

    #repeat test the require number of times to get correct avg
    for take in range (0 , rep_number):
        num_of_points = starting_value + step_width*comb
        result = subprocess.check_output(
                'echo "' + str(num_of_points) + ' ' + str(coordinates_range) 
                + '" | ./generator/generator | ./tester', shell=True)

        #CGAL_result = subprocess.check_output(??)
        #evaluate correctness TODO

        curr_time = int(result.split('\n')[0].split(' ')[1])
        time_accumulator += curr_time

    #compute avg of different takes to get a valid result
    avg_time = time_accumulator/rep_number
    exec_time.append(avg_time)

    #write a line in the csv file
    writer.writerow([num_of_points,avg_time])

ofile.close()

#plot results
x = np.arange(starting_value, (starting_value + step_width*comb_number) , step_width)
y = np.array(exec_time)

plt.title("Number of input points")
plt.plot(x, y, 'b-', linewidth = 2.0)
plt.ylabel('Execution time [us]')
plt.xlabel('Number of input points')
plt.grid(True)
plt.show()
