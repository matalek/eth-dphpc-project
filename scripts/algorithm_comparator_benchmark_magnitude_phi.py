#!/usr/bin/env python
#SBATCH --gres=mic:1 -n 64 -N 1


import subprocess
import csv
import sys

# Usage ./algorithm_comparator.py
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

subprocess.call('export SINK_LD_LIBRARY_PATH=/opt/intel/composer_xe_2015/lib/mic/', shell=True)
subprocess.call('export OMP_NUM_THREADS=64', shell=True)

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

# Store parameters passed by user
CONST_REP_NUMBER = 20
CONST_COORDINATE_RANGE = 4000000000
CONST_POINTS = [100, 1000, 10000, 100000, 1000000, 10000000, 100000000]

# Initialize the map from algorithm to execution times
algorithms_map = {}

# Build executables
subprocess.call('make', shell=True)
subprocess.call('make -C generator/', shell=True)
subprocess.call('mkdir -p log_files', shell=True)
subprocess.call('icc -mmic -fopenmp -std=c++11 tester.cc -o test.mic', shell=True)

# Generate empty CSVs for eveery algorithm tested
for key in range(0, len(sys.argv) - 2):
    algorithm = sys.argv[2 + key]

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

i = 0
# Start tests
for num_of_points in CONST_POINTS:
    i += 1
    # Print progress information to screen
    print ('\n-----------------------------------------\nBEGINNING STEP: ' +
           str(i) +
           ', POINTS:' + str(num_of_points) + '\n-----------------------------------------'
           )

    # Repeat each test CONST_REP_NUMBER times to lower effect of randomicity
    for repetition in range(0, CONST_REP_NUMBER):

        # Print progress information to screen
        print '\nGenerating points for TAKE ' + str(repetition + 1) + '...\n'

        # Generate input points
        subprocess.call(
            'echo "' + str(num_of_points) + ' ' + str(CONST_COORDINATE_RANGE)
            + '" | ./generator/generator > /micfs/team08/tmp.log', shell=True)

        # Apply CGAL_algorithm to compare our result to
        cgal_result = subprocess.check_output(
            'srun --gres=mic:1 micnativeloadex ./test.mic -a Sequential:1', shell=True)

        # Apply every given algorithm to the set of points
        for key in range(0, len(sys.argv) - 2):
            algorithm = sys.argv[2 + key]
            algorithm_name = algorithm.split(":")[0]
            concurrency = algorithm.split(":")[1]

            # Print progress information to screen
            print(algorithm + ' REP ' + str(repetition + 1))

            # Apply given algorithm. Output of algorithm: time\n resulting_points
            alg_result = subprocess.check_output(
                'srun --gres=mic:1 micnativeloadex ./test.mic -a ' + algorithm_name + ':' + concurrency, shell=True)

            # evaluate correctness on points array
            if not compare_function(cgal_result, alg_result):
                print 'ERROR, ALGORITHM INCORRECT\nALGORITHM:\n' + alg_result + '\nCGAL:\n' + cgal_result
                sys.exit()

            # Update tmp store
            algorithms_map[algorithm].append(int(alg_result.split('\n')[0].split(' ')[1]))

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
