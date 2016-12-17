#!/usr/bin/env python
import Algorithm
import operator
import csv
import sys
import numpy as np
import matplotlib.pyplot as plt
import math

# Usage ./plotter_benchmark.py
# -a <algorithms to compare in the format <algo1:num_of_threads algo2:num_of_threads algo3:num_of_threads ...>>

CONST_ALGORITHMS = ['SimpleParallel', 'NaiveParallel', 'HullTree']
CONST_POINTS = 1000000
CONST_THREADS = [2, 4, 8, 16, 32]

def build_algorithms():
    algorithms = {}
    # Build the map of algorithms
    for algorithm_name in CONST_ALGORITHMS:

        for num_of_threads in CONST_THREADS:
            algorithm = Algorithm.Algorithm(algorithm_name + ':' + str(num_of_threads))
            flag = -1

            with open('./log_files_euler/euler_log_results_' + algorithm.name.replace("/", "_").replace(":", "_t_") +
                              '.csv', 'rb') as f:
                reader = csv.reader(f)
                for row in reader:
                    # skip first row
                    if flag == -1:
                        flag += 1
                        continue

                    num_of_input_points = (int(row[0]))

                    # Build array containing all measured values
                    measured_execution_times = []
                    for value in row[2:]:
                        measured_execution_times.append(float(value))

                    algorithm.execution_time[num_of_input_points] = measured_execution_times

            algorithms[algorithm.name] = algorithm

    # Build Sequential
    algorithm = Algorithm.Algorithm('Sequential:1')
    flag = -1

    with open('./log_files_euler/euler_log_results_' + algorithm.name.replace("/", "_").replace(":", "_t_") +
                      '.csv', 'rb') as f:
        reader = csv.reader(f)
        for row in reader:
            # skip first row
            if flag == -1:
                flag += 1
                continue

            num_of_input_points = (int(row[0]))

            # Build array containing all measured values
            measured_execution_times = []
            for value in row[2:]:
                measured_execution_times.append(float(value))

            algorithm.execution_time[num_of_input_points] = measured_execution_times

    algorithms[algorithm.name] = algorithm

    return algorithms


def calculate_boundary(algorithm_name):

    # Uncomment appropriate line depending on input data shape
    expected_size = lambda n : math.log(n, 2)
    # expected_size = lambda n : 100 * n**(1/3)
    # expected_size = lambda n : n

    if algorithm_name == 'NaiveParallel':
        overhead = lambda threads : 2 * math.log(threads) * expected_size(CONST_POINTS / threads)
    elif algorithm_name == 'SimpleParallel':
        overhead = lambda threads : 2 * threads * math.log(CONST_POINTS / threads, 2)**2
    elif algorithm_name == 'HullTree':
        overhead = lambda threads : 10 * math.log(threads, 2) + math.log(CONST_POINTS / threads, 2)

    boundary = lambda threads : CONST_POINTS / (CONST_POINTS / threads + overhead(threads))

    return map(boundary, CONST_THREADS)


colors = ['b', 'r', 'g', 'k', 'y', 'c']

# Map of algortihms. The map will be {algorithm_name, Algorithm}
algorithms = build_algorithms()

# Execution Time plotter -------------------------------------------------------------------------------------------

plt.title("Performance comparison")
plt.ylabel('Response Time [s]')
plt.xlabel('Number of input points')
plt.grid(True)
count = 0
for algorithm_name in sys.argv[2:]:
    curr_algorithm = algorithms[algorithm_name]

    # Make axis start from 0
    num_of_input_points = [0]
    mean_execution_time = [0.0]
    stdv = [0.0]

    # Sort in increasing number of points
    sorted_num_of_points = sorted(curr_algorithm.execution_time.items(), key=operator.itemgetter(0))

    for points in sorted_num_of_points:

        curr_num_of_points = points[0]
        num_of_input_points.append(curr_num_of_points)

        measured_execution_time = curr_algorithm.execution_time[curr_num_of_points]

        mean_execution_time.append(float(np.mean(measured_execution_time) / (10 ** 6)))
        stdv.append(float(np.std(measured_execution_time) / (10 ** 6)))

    if algorithm_name == 'Sequential:1':
        my_label = 'Sequential'
    else:
        my_label = algorithm_name.replace("_", " ").replace(":", " ") + ' threads'

    plt.plot(num_of_input_points, mean_execution_time, colors[count] + 'o--', label=my_label)
    plt.errorbar(num_of_input_points, mean_execution_time, stdv, ecolor=colors[count], fmt='|')
    count += 1

plt.legend(loc=2)
plt.ylim()
plt.savefig('./logs_plots/' + ("&".join(sys.argv[8:])).replace("/", "_") + '.png')
plt.show()


# Speedup plot -------------------------------------------------------------------------------------------------

plt.clf()

plt.title("Speedup")
plt.ylabel('Speedup')
plt.xlabel('Number of input points')
plt.grid(True)
sequential_algorithm = algorithms['Sequential:1']
count = 0
maximum_sp = 0
for algorithm_name in sys.argv[2:]:

    if algorithm_name != 'Sequential:1':

        curr_algorithm = algorithms[algorithm_name]
        speedup = []
        num_of_input_points = []

        # Sort in increasing number of points
        sorted_num_of_points = sorted(curr_algorithm.execution_time.items(), key=operator.itemgetter(0))

        for points in sorted_num_of_points:
            curr_num_of_points = points[0]
            num_of_input_points.append(curr_num_of_points)

            sequential_exec_time = sequential_algorithm.execution_time[curr_num_of_points]
            alg_execution_time = curr_algorithm.execution_time[curr_num_of_points]

            mean_sequential_exec_time = np.mean(sequential_exec_time)
            mean_execution_time = np.mean(alg_execution_time)

            sp = mean_sequential_exec_time / mean_execution_time

            if sp > maximum_sp:
                maximum_sp = sp

            speedup.append(mean_sequential_exec_time / mean_execution_time)

        my_label = algorithm_name.replace("_", " ").replace(":", " ") + ' threads'

        plt.plot(num_of_input_points, speedup, colors[count] + 'o--', label=my_label)
        count += 1

plt.legend(loc=3)
plt.ylim(ymin=0, ymax=int(maximum_sp) + 1)
plt.savefig('./logs_plots/speedup_' + ("&".join(sys.argv[8:])).replace("/", "_") + '.png')
plt.show()

# Plot execution time x numof threads-----------------------------------------------------------------------------

plt.title("Performance comparison")
plt.ylabel('Response Time [s]')
plt.xlabel('Number of threads')

count = 0
for algorithm_name in CONST_ALGORITHMS:


    mean_execution_time = []
    stdv = []
    my_label = algorithm_name
    for n_threads in CONST_THREADS:
        curr_algorithm = algorithms[algorithm_name + ':' + str(n_threads)]
        measured_execution_time = curr_algorithm.execution_time[CONST_POINTS]

        mean_execution_time.append(float(np.mean(measured_execution_time) / (10 ** 6)))
        stdv.append(float(np.std(measured_execution_time) / (10 ** 6)))

    plt.plot(CONST_THREADS, mean_execution_time, colors[count] + 'o--', label=my_label)
    count += 1

# Sequential
mean_execution_time = []
stdv = []
my_label = 'Sequential'

seq_algorithm = algorithms['Sequential:1']
sequential_exec_time = seq_algorithm.execution_time[CONST_POINTS]
mean_sequential_exec_time = float(np.mean(sequential_exec_time) / (10 ** 6))
sequential_stdv = float(np.std(sequential_exec_time) / (10 ** 6))
for n_threads in CONST_THREADS:
    mean_execution_time.append(mean_sequential_exec_time)
    stdv.append(sequential_stdv)

plt.plot(CONST_THREADS, mean_execution_time, colors[count] + 'o--', label=my_label)
plt.xticks(CONST_THREADS)
plt.grid(True)
plt.legend(loc=1)
plt.show()
plt.clf()

# Plot Speedup x numof threads-----------------------------------------------------------------------------

plt.title("Performance comparison")
plt.ylabel('Speedup')
plt.xlabel('Number of threads')

count = 0
for algorithm_name in CONST_ALGORITHMS:

    mean_execution_time = []
    stdv = []
    my_label = algorithm_name
    for n_threads in CONST_THREADS:
        curr_algorithm = algorithms[algorithm_name + ':' + str(n_threads)]
        measured_execution_time = curr_algorithm.execution_time[CONST_POINTS]

        mean_execution_time.append(mean_sequential_exec_time / float(np.mean(measured_execution_time) / (10 ** 6)))
        stdv.append(float(np.std(measured_execution_time) / (10 ** 6)))

    plt.plot(CONST_THREADS, mean_execution_time, colors[count] + 'o--', label=my_label)
    count += 1

plt.xticks(CONST_THREADS)
plt.grid(True)
plt.legend(loc=1)
plt.show()

# Plot Speedup x numof threads with theoretical boundaries-----------------------------------------------------------------------------

plt.title("Performance comparison")
plt.ylabel('Speedup')
plt.xlabel('Number of threads')

for algorithm_name in CONST_ALGORITHMS:
    mean_execution_time = []
    stdv = []
    my_label = algorithm_name
    for n_threads in CONST_THREADS:
        curr_algorithm = algorithms[algorithm_name + ':' + str(n_threads)]
        measured_execution_time = curr_algorithm.execution_time[CONST_POINTS]

        mean_execution_time.append(mean_sequential_exec_time / float(np.mean(measured_execution_time) / (10 ** 6)))
        stdv.append(float(np.std(measured_execution_time) / (10 ** 6)))

    plt.plot(CONST_THREADS, mean_execution_time, 'bo--', label=my_label)

    plt.plot(CONST_THREADS, calculate_boundary(algorithm_name), 'r')
    count += 1

    plt.xticks(CONST_THREADS)
    plt.grid(True)
    plt.legend(loc=1)
    plt.show()
