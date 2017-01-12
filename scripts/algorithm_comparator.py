#!/usr/bin/env python
import subprocess
import csv
import sys

# Usage ./algorithm_comparator.py
# -c <number of different combinations of number of points> 
# -w <width of steps> or -m <base of the magnitude comparison (ex 10 for 100, 1000, 10000 ...)>
# -s <starting number of points>
# -r <range of points coordinates>
# -R <number of repetition for each number of points>
# -S <shape (circle, square, disk)>
# -a <algorithms to compare in the format <algo1:num_of_threads algo2:num_of_threads algo3:num_of_threads ...>>


# Comparator to ensure correctness of result
def compare_function(cgal, alg):
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


# Create the map for algorithms
def algorithms_map():
    key_val = {}
    # Generate empty CSVs for eveery algorithm tested
    for key in range(0, len(sys.argv) - 14):
        algorithm = sys.argv[14 + key]
        key_val[algorithm] = []
        for suffix in CONST_CSV_SUFFIXES:
            # create and open a csv file to store results
            ofile = open('log_files/' + algorithm.replace(':', '_') + '_' + CONST_SHAPE + suffix + '.csv', "wb")
            writer = csv.writer(ofile)
            writer.writerow(['#Input Points', '', 'Exec_Time [us]'])
            ofile.close()
            key_val[algorithm].append([])
    return key_val


# Find if desired linear or exponential width
def chosen_width_type():
    if '-w' in sys.argv:
        return '-w'
    print 'no'
    return '-m'


# Insert function check_output if not present
if "check_output" not in dir(subprocess ):
    def f(*popenargs, **kwargs):
        if 'stdout' in kwargs:
            raise ValueError('stdout argument not allowed, it will be overridden.')
        process = subprocess.Popen(stdout=subprocess.PIPE, *popenargs, **kwargs)
        output, unused_err = process.communicate()
        retcode = process.poll()
        if retcode:
            cmd = kwargs.get("args")
            if cmd is None:
                cmd = popenargs[0]
            raise subprocess.CalledProcessError(retcode, cmd)
        return output
    subprocess.check_output = f


# Check user input correctness
def check_input_correctness():
    if len(sys.argv) <= 15:
        print 'ERROR in usage, provide all the required arguments'
        return False
    if '-R' not in sys.argv:
        print 'ERROR in usage, provide int number of repetition for each value (ex. -R 20)'
        return False
    if '-r' not in sys.argv:
        print 'ERROR in usage, provide int range of points coordinates (ex. -r 20)'
        return False
    if '-s' not in sys.argv:
        print 'ERROR in usage, provide int starting number of points (ex. -s 100)'
        return False
    if '-S' not in sys.argv:
        print 'ERROR in usage, provide shape (ex. -S square)'
        return False
    if '-w' not in sys.argv and '-m' not in sys.argv:
        print 'ERROR in usage, provide int width in steps or magnitude base (ex. -w 500 or -m 10)'
        return False
    if '-c' not in sys.argv:
        print 'ERROR in usage, provide int number of different combinations (ex. -c 20)'
        return False
    if '-a' not in sys.argv:
        print 'ERROR in usage, provide at least one algorithm (ex. -a Sequential:1)'
        return False
    if sys.argv.index('-a') != 14:
        print 'ERROR in usage, please provide algorithms at the end of arguments'
        return False
    return True


# Start program
if not check_input_correctness():
    exit

# Store parameters passed by user
CONST_SHAPE = sys.argv[sys.argv.index('-S') + 1]
CONST_REP_NUMBER = int(sys.argv[sys.argv.index('-R') + 1])
CONST_COORDINATE_RANGE = int(sys.argv[sys.argv.index('-r') + 1])
CONST_STARTING_VALUE = int(sys.argv[sys.argv.index('-s') + 1])
CONST_STEP_WIDTH = int(sys.argv[sys.argv.index(chosen_width_type()) + 1])
CONST_COMB_NUMBER = int(sys.argv[sys.argv.index('-c') + 1])
CONST_CSV_SUFFIXES = ['', '_mid', '_end']

# Build executables
subprocess.call('make clean', shell=True)
subprocess.call('make', shell=True)
subprocess.call('make -C generator/', shell=True)
subprocess.call('mkdir -p log_files', shell=True)
subprocess.call('(cd cgal && cmake .)', shell=True)
subprocess.call('(cd cgal && make)', shell=True)

# Initialize the algorithms_map containing temp execution times
key_value = algorithms_map()

# Create array of points
points = []
if chosen_width_type() == '-w':
    for n in range(CONST_STARTING_VALUE, CONST_STEP_WIDTH * CONST_COMB_NUMBER + CONST_STARTING_VALUE,
                       CONST_STEP_WIDTH):
        points.append(n)
else:
    for val in range(0, CONST_COMB_NUMBER):
        points.append(CONST_STARTING_VALUE * (CONST_STEP_WIDTH ** val))

# Start tests
for num_of_points in points:

    # Print progress information to screen
    print ('\n-----------------------------------------\nBEGINNING STEP: ' +
           str(points.index(num_of_points) + 1) +
           ', POINTS:' + str(num_of_points) + '\n-----------------------------------------'
           )

    # Repeat each test CONST_REP_NUMBER times to lower effect of randomicity
    for repetition in range(0, CONST_REP_NUMBER):

        # Print progress information to screen
        print '\nGenerating points for TAKE ' + str(repetition + 1) + '...\n'

        # Generate input points
        subprocess.call(
            'echo "' + str(num_of_points) + ' ' + str(CONST_COORDINATE_RANGE)
            + '" | ./generator/generator ' + CONST_SHAPE + ' > tmp.log', shell=True)

        # Apply CGAL_algorithm to compare our result to
        cgal_result = subprocess.check_output(
            'cat tmp.log | ./cgal/cgal_graham_andrew', shell=True)

        # Apply every given algorithm to the set of points
        for algorithm in key_value:

            # Print progress information to screen
            print(algorithm + ' REP ' + str(repetition + 1))

            # Apply given algorithm. Output of algorithm: time\n resulting_points
            alg_result = subprocess.check_output(
                'cat tmp.log | ./tester ' + algorithm, shell=True)

            # evaluate correctness on points array
            if not compare_function(cgal_result, alg_result):
                print 'ERROR, ALGORITHM INCORRECT'
                print 'ALGORITHM:\n' + alg_result
                print 'CGAL:\n' + cgal_result
                sys.exit()

            # Update tmp store
            key_value[algorithm][0].append(int(alg_result.split('\n')[0].split(' ')[1]) + 
                                    int(alg_result.split('\n')[0].split(' ')[2]))
            for i in (1, 2):
                key_value[algorithm][i].append(int(alg_result.split('\n')[0].split(' ')[i]))

    # Write a new row in every CSV with this step's result
    for algorithm in key_value:
        pos = 0
        for suffix in CONST_CSV_SUFFIXES:
            ofile = open('log_files/' + algorithm.replace(':', '_') + '_' + CONST_SHAPE + suffix + '.csv', "a")
            writer = csv.writer(ofile)
            writer.writerow([num_of_points] + [''] + key_value[algorithm][pos])
            key_value[algorithm][pos] = []
            ofile.close()
            pos += 1

# Print progress information to screen
print('\n--------------------------------------\n| ----------------------------------- |\n'
      '| |COMPARISON COMPLETED SUCCESSFULLY| |'
      '\n| ----------------------------------- |\n--------------------------------------\n')
