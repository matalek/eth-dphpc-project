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
def build_algorithms(shape):
    algorithms = {}
    source_file = ('./log_files/log_files_' + CONST_MACHINE + '/' + CONST_MACHINE + '_' + shape + '/' +
                         CONST_MACHINE + '_' + shape + '_')
    # Build the map of algorithms
    for algorithm_name in CONST_ALGORITHMS_NAMES:

        for num_of_threads in CONST_THREADS:
            algorithm = Algorithm.Algorithm(algorithm_name + ':' + str(num_of_threads))
            
            # Overall time
            with open(source_file + algorithm.name.replace("/", "_").replace(":", "_") + '.csv', 'rb') as f:
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
            with open(source_file + algorithm.name.replace("/", "_").replace(":", "_") + '_mid.csv', 'rb') as f:
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
            with open(source_file + algorithm.name.replace("/", "_").replace(":", "_") + '_end.csv', 'rb') as f:
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
    
    with open(source_file + algorithm.name.replace("/", "_").replace(":", "_") + '.csv', 'rb') as f:
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


# Standard execution Time plotter --------------------------------------------------------------------------------------
def plot_execution_time():
    plt.figure(num=None, figsize=(10, 4), facecolor='w', edgecolor='k')
    plt.gcf().subplots_adjust(bottom=0.15)
    plt.suptitle("Performance comparison")

    plt.subplot(121)
    plt.title('Square')
    plt.ylabel('Execution time [s]')
    plt.subplot(122)
    plt.title('Circle')
    for i in [121, 122]:
        plt.subplot(i)
        plt.xlabel('Number of input points')
        plt.grid(True)
        plt.ylim([0.0008, 10])

    subplot_index = 121
    for shape in ['square', 'circle']:
        count = 0
        algorithms = build_algorithms(shape)

        plt.subplot(subplot_index)

        for algorithm_name in ['SimpleParallel:8', 'NaiveParallel:8', 'HullTree:8', 'Sequential:1']:
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

                mean_execution_time.append(float(np.average(measured_execution_time) / (10 ** 6)))
                stdv.append(float(np.std(measured_execution_time) / (10 ** 6)))

            if algorithm_name == 'Sequential:1':
                my_label = 'Sequential'
            else:
                my_label = algorithm_name.replace("_", " ").replace(":8", "")

            plt.loglog(num_of_input_points, mean_execution_time, CONST_COLORS[count] + '--', label=my_label)
            # plt.errorbar(num_of_input_points, mean_execution_time, stdv, ecolor=colors[count], fmt='|')
            count += 1

        subplot_index += 1
    plt.subplot(121)
    plt.legend(loc=2)
    plt.savefig('./logs_plots/time_points.eps', format='eps')
    plt.show()
    plt.clf()


# Standard speedup plotter ---------------------------------------------------------------------------------------------
def plot_speedup():
    plt.figure(num=None, figsize=(10, 4), facecolor='w', edgecolor='k')
    plt.gcf().subplots_adjust(bottom=0.15)
    plt.suptitle("Speedup")
    plt.subplot(121)
    plt.title('Square')
    plt.ylabel('speedup')
    plt.subplot(122)
    plt.title('Circle')
    for i in [121, 122]:
        plt.subplot(i)
        plt.yticks(np.arange(0, 10))
        plt.xlabel('Number of input points')
        plt.ylim([0, 9.2])
        plt.grid(True)

    maximum_sp = 0
    subplot_index = 121
    for shape in ['square', 'circle']:
        count = 0
        plt.subplot(subplot_index)
        algorithms = build_algorithms(shape)
        sequential_algorithm = algorithms['Sequential:1']
        for algorithm_name in ['SimpleParallel:8', 'NaiveParallel:8', 'HullTree:8']:

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

                    mean_sequential_exec_time = np.average(sequential_exec_time)
                    mean_execution_time = np.average(alg_execution_time)

                    sp = mean_sequential_exec_time / mean_execution_time

                    if sp > maximum_sp:
                        maximum_sp = sp

                    speedup.append(mean_sequential_exec_time / mean_execution_time)

                my_label = algorithm_name.replace("_", " ").replace(":8", "")

                plt.semilogx(num_of_input_points, speedup, CONST_COLORS[count] + '--', label=my_label)
                count += 1

        subplot_index += 1
    plt.legend(loc=2)
    plt.savefig('./logs_plots/speedup_points.eps', format='eps')
    plt.show()
    plt.clf()


# Map of algorithms. The map will be {algorithm_name, Algorithm}
algorithms_map = build_algorithms(CONST_SHAPE)

# Sequential algorithm
seq_algorithm = algorithms_map['Sequential:1']

# Execution Time plotter
plot_execution_time()

# Speedup plotter
plot_speedup()

# # Execution Time plotter threads on x axis
# plot_execution_time_fixed_points(algorithms_map, seq_algorithm)
#
# # Speedup plotter threads on x axis
# plot_speedup_fixed_points(algorithms_map, seq_algorithm)
#
# # Plot Speedup x numof threads with theoretical boundaries
# plot_theoretical_boundaries(algorithms_map, seq_algorithm)
#
# # Box Plots for fixed points
# plot_box_time_fixed_points(algorithms_map, seq_algorithm)
#
# # Box Plots for fixed points all algos
# plot_box_all(algorithms_map, seq_algorithm)

