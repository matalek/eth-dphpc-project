#!/usr/bin/env python
import subprocess
import csv
import sys

# Usage ./algorithm_comparator.py
# -c <number of different combinations of number of points> 
# -w <width of steps>
# -s <starting number of points>
# -r <range of points coordinates>
# -R <number of repetition for each number of points>
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


# Store parameters passed by user
CONST_REP_NUMBER = int(sys.argv[10])
CONST_COORDINATE_RANGE = int(sys.argv[8])
CONST_STARTING_VALUE = int(sys.argv[6])
CONST_STEP_WIDTH = int(sys.argv[4])
CONST_COMB_NUMBER = int(sys.argv[2])

# Initialize the map from algorithm to execution times
algorithms_map = {}

# Build executables
subprocess.call('make', shell=True)
subprocess.call('make -C generator/', shell=True)
subprocess.call('mkdir -p log_files', shell=True)

# Generate empty CSVs for eveery algorithm tested
for key in range(0, len(sys.argv) - 12):
    algorithm = sys.argv[12 + key]

    # create and open a csv file to store results
    ofile = open('log_files/log_results_' + algorithm.replace('/', '_').replace(':', '_t_') + '.csv', "wb")
    writer = csv.writer(ofile)
    columns_title = ['#Input Points', '']
    for i in range(0, CONST_REP_NUMBER):
        columns_title.append('Exec_Time[us]_R_' + str(i + 1))
    writer.writerow(columns_title)
    ofile.close()

    # Initialize the map between algorithm and execution times
    algorithms_map[algorithm] = []

# Start tests
for num_of_points in range(
        CONST_STARTING_VALUE, CONST_STEP_WIDTH*CONST_COMB_NUMBER + CONST_STARTING_VALUE, CONST_STEP_WIDTH):

    # Print progress information to screen
    print ('\n-----------------------------------------\nBEGINNING STEP: ' +
           str(((num_of_points-CONST_STARTING_VALUE)/CONST_STEP_WIDTH)+1) +
           ', POINTS:' + str(num_of_points) + '\n-----------------------------------------'
           )

    # Repeat each test CONST_REP_NUMBER times to lower effect of randomicity
    for repetition in range(0, CONST_REP_NUMBER):

        # Print progress information to screen
        print '\nGenerating points for TAKE ' + str(repetition + 1) + '...\n'

        # Generate input points
        subprocess.call(
            'echo "' + str(num_of_points) + ' ' + str(CONST_COORDINATE_RANGE)
            + '" | ./generator/generator > tmp.log', shell=True)

        # Apply CGAL_algorithm to compare our result to
        cgal_result = subprocess.check_output(
            'cat tmp.log | ./tester Squential:1', shell=True)

        # Apply every given algorithm to the set of points
        for key in range(0, len(sys.argv) - 12):
            algorithm = sys.argv[12 + key]
            algorithm_name = algorithm.split(":")[0]
            concurrency = algorithm.split(":")[1]

            # Print progress information to screen
            print(algorithm + ' REP ' + str(repetition + 1))

            # Apply given algorithm. Output of algorithm: time\n resulting_points
            alg_result = subprocess.check_output(
                'cat tmp.log | ./tester ' + algorithm_name + ':' + concurrency, shell=True)

            # evaluate correctness on points array
            if not compare_function(cgal_result, alg_result):
                print 'ERROR, ALGORITHM INCORRECT\nALGORITHM:\n' + alg_result + '\nCGAL:\n' + cgal_result
                sys.exit()

            # Update tmp store
            algorithms_map[algorithm].append(int(alg_result.split('\n')[0].split(' ')[1]))

    # Delete tmp file containing the points
    subprocess.call('rm tmp.log', shell=True)

    # Write a new row in every CSV with this step's result
    for algorithm in algorithms_map:
        ofile = open('log_files/log_results_' + algorithm.replace('/', '_').replace(':', '_t_') + '.csv', "a")
        writer = csv.writer(ofile)
        writer.writerow([num_of_points, ''] + algorithms_map[algorithm])
        algorithms_map[algorithm] = []
        ofile.close()

# Print progress information to screen
print('\n--------------------------------------\n| ----------------------------------- |\n'
      '| |COMPUTATION COMPLETED SUCCESSFULLY| |'
      '\n| ----------------------------------- |\n--------------------------------------\n')
