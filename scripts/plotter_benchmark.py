#!/usr/bin/env python
import Algorithm
import operator
import csv
import sys
import numpy as np
import matplotlib.pyplot as plt

# Usage ./plotter_benchmark.py
# -a <algorithms to compare in the format <algo1:num_of_threads algo2:num_of_threads algo3:num_of_threads ...>>

colors = ['b', 'r', 'g', 'k', 'y']

# Map of algortihms. The map will be {algorithm_name, Algorithm}
algorithms = {}

count = 0

# Build the map of algorithms
for index in range(2, len(sys.argv)):
    algorithm = Algorithm.Algorithm(sys.argv[index])
    flag = -1

    with open('./log_files/log_results_' + algorithm.name.replace("/", "_").replace(":", "_t_") + '.csv', 'rb') as f:
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

# Execution Time plotter -------------------------------------------------------------------------------------------

plt.title("Performance comparison")
plt.ylabel('Response Time [s]')
plt.xlabel('Number of input points')
plt.grid(True)

for algorithm_name in algorithms:
    curr_algorithm = algorithms[algorithm_name]
    num_of_input_points = []
    mean_execution_time = []
    stdv = []

    # Sort in increasing number of points
    sorted_num_of_points = sorted(curr_algorithm.execution_time.items(), key=operator.itemgetter(0))

    for points in sorted_num_of_points:

        curr_num_of_points = points[0]
        num_of_input_points.append(curr_num_of_points)

        measured_execution_time = curr_algorithm.execution_time[curr_num_of_points]

        mean_execution_time.append(np.mean(measured_execution_time) / (10 ** 6))
        stdv.append(np.std(measured_execution_time) / (10 ** 6))

    if algorithm_name == 'Sequential:1':
        my_label = 'Sequential'
    else:
        my_label = algorithm_name.replace("_", " ").replace(":", " ") + ' threads'

    plt.plot(num_of_input_points, mean_execution_time, colors[count] + 'o--', label=my_label)
    plt.errorbar(num_of_input_points, mean_execution_time, stdv, ecolor=colors[count], fmt='|')
    plt.yticks(mean_execution_time)
    count += 1

plt.legend(loc=2)

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

for algorithm_name in algorithms:

    if algorithm_name != 'Sequential:1':

        curr_algorithm = algorithms[algorithm_name]
        speedup = []
        num_of_input_points = []

        # Sort in increasing number of points
        sorted_num_of_points = sorted(curr_algorithm.execution_time.items(), key=operator.itemgetter(0))

        for points in sorted_num_of_points:
            print points[0]
            curr_num_of_points = points[0]
            num_of_input_points.append(curr_num_of_points)

            sequential_exec_time = sequential_algorithm.execution_time[curr_num_of_points]
            alg_execution_time = curr_algorithm.execution_time[curr_num_of_points]

            mean_sequential_exec_time = np.mean(sequential_exec_time)
            mean_execution_time = np.mean(alg_execution_time)

            print mean_sequential_exec_time
            print mean_execution_time

            speedup.append(mean_sequential_exec_time / mean_execution_time)

        my_label = algorithm_name.replace("_", " ").replace(":", " ") + ' threads'

        plt.plot(num_of_input_points, speedup, colors[count] + 'o--', label=my_label)
        count += 1

plt.legend(loc=2)
plt.savefig('./logs_plots/speedup_' + ("&".join(sys.argv[8:])).replace("/", "_") + '.png')
plt.show()
