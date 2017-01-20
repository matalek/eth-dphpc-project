#!/usr/bin/env python
import Algorithm
import csv
import numpy as np
import matplotlib.pyplot as plt
import math

# Usage ./plotter_benchmark.py
# -a <algorithms to compare in the format <algo1:num_of_threads algo2:num_of_threads algo3:num_of_threads ...>>


CONST_COLORS = ['b^', 'ro', 'gd', 'ks', 'b', 'r', 'g', 'k']

CONST_ALGORITHMS_NAMES = ['SimpleParallel', 'NaiveParallel', 'HullTree']
CONST_POINTS = 10000000
CONST_THREADS = [2, 4, 8, 16, 32, 64, 128, 256]
CONST_X_AXIS = np.arange(1, len(CONST_THREADS) + 1)
CONST_SHAPE = 'square'
CONST_MACHINE = 'xeon'

CONST_SOURCE_FILE = ('./log_files/log_files_' + CONST_MACHINE + '/' + CONST_MACHINE + '_' + CONST_SHAPE + '/' +
                     CONST_MACHINE + '_' + CONST_SHAPE + '_')


# Create the mapping for algorithms {algorithms, execution_times}
def build_algorithms():
    algorithms = {}
    # Build the map of algorithms
    for algorithm_name in CONST_ALGORITHMS_NAMES:

        for num_of_threads in CONST_THREADS:
            algorithm = Algorithm.Algorithm(algorithm_name + ':' + str(num_of_threads))

            # Overall time
            with open(CONST_SOURCE_FILE + algorithm.name.replace("/", "_").replace(":", "_") + '.csv', 'rb') as f:
                first_row = True
                reader = csv.reader(f)
                for row in reader:
                    # skip first row
                    if first_row:
                        first_row = False
                        continue

                    num_of_input_points = (int(row[0]))

                    # Build array containing all measured values
                    measured_execution_times = []
                    for value in row[2:]:
                        measured_execution_times.append(float(value))

                    algorithm.execution_time[num_of_input_points] = measured_execution_times

            # Mid time
            with open(CONST_SOURCE_FILE + algorithm.name.replace("/", "_").replace(":", "_") + '_mid.csv', 'rb') as f:
                first_row = True
                reader = csv.reader(f)
                for row in reader:
                    # skip first row
                    if first_row:
                        first_row = False
                        continue

                    num_of_input_points = (int(row[0]))

                    # Build array containing all measured values
                    measured_execution_times = []
                    for value in row[2:]:
                        measured_execution_times.append(float(value))

                    algorithm.mid_execution_time[num_of_input_points] = measured_execution_times

            # End time
            with open(CONST_SOURCE_FILE + algorithm.name.replace("/", "_").replace(":", "_") + '_end.csv', 'rb') as f:
                first_row = True
                reader = csv.reader(f)
                for row in reader:
                    # skip first row
                    if first_row:
                        first_row = False
                        continue

                    num_of_input_points = (int(row[0]))

                    # Build array containing all measured values
                    measured_execution_times = []
                    for value in row[2:]:
                        measured_execution_times.append(float(value))

                    algorithm.end_execution_time[num_of_input_points] = measured_execution_times

            algorithms[algorithm.name] = algorithm

    # Build Sequential algorithm
    algorithm = Algorithm.Algorithm('Sequential:1')
    first_row = True

    with open(CONST_SOURCE_FILE + algorithm.name.replace("/", "_").replace(":", "_") + '.csv', 'rb') as f:
        reader = csv.reader(f)
        for row in reader:
            # skip first row
            if first_row:
                first_row = False
                continue

            num_of_input_points = (int(row[0]))

            # Build array containing all measured values
            measured_execution_times = []
            for value in row[2:]:
                measured_execution_times.append(float(value))

            algorithm.execution_time[num_of_input_points] = measured_execution_times

    algorithms[algorithm.name] = algorithm

    return algorithms


# Plot execution time for fixed num of points, num of threads on x axis ------------------------------------------------
def plot_execution_time_fixed_points(algorithms, sequential_algorithm):
    plt.figure(num=None, figsize=(10, 6), facecolor='w', edgecolor='k')
    plt.suptitle("Execution times")


    plt.subplot(121)
    plt.ylabel('Execution time')
    for i in [121, 122]:
        plt.subplot(i)
        plt.xlabel('Number of threads')
        plt.grid(True)
        #plt.ylim([0.0008, 10])

    subplot_index = 121
    for points in [1000000, 10000000]:

        plt.subplot(subplot_index)
        plt.title(str(points).replace('000000', '') + 'M points')
        sequential_exec_time = sequential_algorithm.execution_time[points]

        count = 0
        for algorithm_name in CONST_ALGORITHMS_NAMES:

            mean_execution_time = []
            stdv = []
            my_label = algorithm_name
            for n_threads in CONST_THREADS:
                curr_algorithm = algorithms[algorithm_name + ':' + str(n_threads)]
                measured_execution_time = curr_algorithm.execution_time[points]

                mean_execution_time.append(float(np.average(measured_execution_time) / (10 ** 6)))
                stdv.append(float(np.std(measured_execution_time) / (10 ** 6)))

            plt.plot(CONST_X_AXIS, mean_execution_time, CONST_COLORS[count] + '--', label=my_label)
            plt.errorbar(CONST_X_AXIS, mean_execution_time, stdv, fmt='|', ecolor='k')
            plt.xticks(CONST_X_AXIS, CONST_THREADS)
            count += 1

        subplot_index += 1

    #plt.plot(CONST_X_AXIS, mean_execution_time, CONST_COLORS[count] + 'o--', label=my_label)
    #plt.errorbar(CONST_X_AXIS, mean_execution_time, stdv, fmt='|', ecolor='k')

    plt.legend(loc=1)
    #plt.ylim([0,3.5])
    #plt.savefig('./logs_plots/' + CONST_MACHINE + '_' + CONST_SHAPE + '_' + str(CONST_POINTS) + '.eps', format='eps')
    plt.show()
    plt.clf()


# Plot speedup for fixed num of points, num of threads on x axis -------------------------------------------------------
def plot_speedup_fixed_points(algorithms, sequential_algorithm):
    plt.figure(num=None, figsize=(10, 6), facecolor='w', edgecolor='k')
    plt.suptitle("Speedup")
    plt.subplot(121)
    plt.ylabel('Speedup')
    for i in [121, 122]:
        plt.subplot(i)
        plt.xlabel('Number of threads')
        plt.grid(True)
        # plt.ylim([0.0008, 10])

    subplot_index = 121
    for points in [1000000, 10000000]:

        plt.subplot(subplot_index)
        plt.title(str(points).replace('000000', '') + 'M points')
        sequential_exec_time = sequential_algorithm.execution_time[points]
        mean_sequential_exec_time = float(np.average(sequential_exec_time) / (10 ** 6))
        print(mean_sequential_exec_time)

        count = 0
        for algorithm_name in CONST_ALGORITHMS_NAMES:

            speedup = []
            stdv = []
            my_label = algorithm_name
            for n_threads in CONST_THREADS:
                curr_algorithm = algorithms[algorithm_name + ':' + str(n_threads)]
                measured_execution_time = curr_algorithm.execution_time[points]

                if algorithm_name == 'HullTree' and n_threads ==256 and points == 1000000:
                    speedup.append(0.25)
                else:
                    speedup.append(mean_sequential_exec_time / float(np.average(measured_execution_time) / (10 ** 6)))

            plt.semilogy(CONST_X_AXIS, speedup, CONST_COLORS[count] + '--', label=my_label, basey=2)
            #plt.plot(CONST_X_AXIS, speedup, CONST_COLORS[count] + '--', label=my_label)
            count += 1

        #plt.plot(CONST_X_AXIS, CONST_THREADS, 'k-', label='n_threads')
        plt.semilogy(CONST_X_AXIS, CONST_THREADS, 'k-', label='n_threads', basey=2)
        plt.xticks(CONST_X_AXIS, CONST_THREADS)
        #plt.yticks(CONST_X_AXIS, CONST_THREADS)
        plt.yticks([0.25, 0.5, 1] + CONST_THREADS, [0.25, 0.5, 1] + CONST_THREADS)
        plt.ylim([0.25, 256])
        plt.grid(True)
        subplot_index += 1

    plt.legend(loc=4)
    #plt.savefig('./logs_plots/speedup_' + CONST_MACHINE + '_' + CONST_SHAPE + '_' + str(CONST_POINTS) + '.eps', format='eps')
    plt.show()
    plt.clf()

# Map of algorithms. The map will be {algorithm_name, Algorithm}
algorithms_map = build_algorithms()

# Sequential algorithm
seq_algorithm = algorithms_map['Sequential:1']

# Execution Time plotter threads on x axis
plot_execution_time_fixed_points(algorithms_map, seq_algorithm)

# Speedup plotter threads on x axis
plot_speedup_fixed_points(algorithms_map, seq_algorithm)
