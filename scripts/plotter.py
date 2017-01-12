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


CONST_COLORS = ['b', 'r', 'g', 'k', 'b', 'r', 'g', 'k']

CONST_ALGORITHMS_NAMES = ['SimpleParallel', 'NaiveParallel', 'HullTree']
CONST_POINTS = 1000000
CONST_THREADS = [2, 4, 8, 16, 32, 64, 128, 256]
CONST_X_AXIS = np.arange(1, len(CONST_THREADS) + 1)
CONST_SHAPE = 'circle'
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


# Compute theoretical boundaries
def calculate_boundary(algorithm_name):

    # Uncomment appropriate line depending on input data shape
    if CONST_SHAPE == 'square':
        expected_size = lambda n: math.log(n, 2)
    elif CONST_SHAPE == 'disk':
        expected_size = lambda n: 100 * n**(1/3)
    elif CONST_SHAPE == 'circle':
        expected_size = lambda n: n

    if algorithm_name == 'NaiveParallel':
        overhead = lambda threads: 2 * math.log(threads) * expected_size(CONST_POINTS / threads)
    elif algorithm_name == 'SimpleParallel':
        overhead = lambda threads: 2 * threads * math.log(CONST_POINTS / threads, 2)**2
    elif algorithm_name == 'HullTree':
        overhead = lambda threads: 10 * math.log(threads, 2) + math.log(CONST_POINTS / threads, 2)

    boundary = lambda threads : CONST_POINTS / (CONST_POINTS / threads + overhead(threads))

    return map(boundary, CONST_THREADS)


# Standard execution Time plotter --------------------------------------------------------------------------------------
def plot_execution_time(algorithms):
    plt.title("Performance comparison")
    plt.ylabel('Execution Time [s]')
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

            mean_execution_time.append(float(np.average(measured_execution_time) / (10 ** 6)))
            stdv.append(float(np.std(measured_execution_time) / (10 ** 6)))

        if algorithm_name == 'Sequential:1':
            my_label = 'Sequential'
        else:
            my_label = algorithm_name.replace("_", " ").replace(":", " ") + ' threads'

        plt.loglog(num_of_input_points, mean_execution_time, CONST_COLORS[count] + 'o--', label=my_label)
        # plt.errorbar(num_of_input_points, mean_execution_time, stdv, ecolor=colors[count], fmt='|')
        count += 1

    plt.legend(loc=2)
    # plt.ylim()
    plt.savefig('./logs_plots/' + ("&".join(sys.argv[2:])).replace("/", "_").replace(":", "_") + '.eps', format='eps')
    plt.show()
    plt.clf()


# Standard speedup plotter ---------------------------------------------------------------------------------------------
def plot_speedup(algorithms, sequential_algorithm):

    plt.title("Speedup")
    plt.ylabel('Speedup')
    plt.xlabel('Number of input points')
    plt.grid(True)
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

                mean_sequential_exec_time = np.average(sequential_exec_time)
                mean_execution_time = np.average(alg_execution_time)

                sp = mean_sequential_exec_time / mean_execution_time

                if sp > maximum_sp:
                    maximum_sp = sp

                speedup.append(mean_sequential_exec_time / mean_execution_time)

            my_label = algorithm_name.replace("_", " ").replace(":", " ") + ' threads'

            plt.semilogx(num_of_input_points, speedup, CONST_COLORS[count] + 'o--', label=my_label)
            count += 1

    plt.legend(loc=4)
    plt.ylim(ymin=0, ymax=int(maximum_sp) + 1)
    # plt.savefig('./logs_plots/speedup_' + ("&".join(sys.argv[8:])).replace("/", "_") + '.png')
    plt.show()
    plt.clf()


# Plot execution time for fixed num of points, num of threads on x axis ------------------------------------------------
def plot_execution_time_fixed_points(algorithms, sequential_algorithm):

    sequential_exec_time = sequential_algorithm.execution_time[CONST_POINTS]
    mean_sequential_exec_time = float(np.average(sequential_exec_time) / (10 ** 6))
    sequential_stdv = float(np.std(sequential_exec_time) / (10 ** 6))

    plt.title("Performance comparison")
    plt.ylabel('Execution Time [s]')
    plt.xlabel('Number of threads')

    count = 0
    for algorithm_name in CONST_ALGORITHMS_NAMES:

        mean_execution_time = []
        stdv = []
        my_label = algorithm_name
        for n_threads in CONST_THREADS:
            curr_algorithm = algorithms[algorithm_name + ':' + str(n_threads)]
            measured_execution_time = curr_algorithm.execution_time[CONST_POINTS]
	    start_execution_time = curr_algorithm.mid_execution_time[CONST_POINTS]
	    end_execution_time = curr_algorithm.end_execution_time[CONST_POINTS]

            mean_execution_time.append(float(np.average(measured_execution_time) / (10 ** 6)))
            stdv.append(float(np.std(measured_execution_time) / (10 ** 6)))

	    print(algorithm_name + ' ' + str(n_threads) + ' ' + CONST_SHAPE + ': ' + str(float(np.average(start_execution_time) / (10 ** 6))) + ' ' + str(float(np.average(end_execution_time) / (10 ** 6))))

        plt.plot(CONST_X_AXIS, mean_execution_time, CONST_COLORS[count] + 'o--', label=my_label)
        plt.errorbar(CONST_X_AXIS, mean_execution_time, stdv, fmt='|', ecolor='k')
        count += 1

    # Sequential
    mean_execution_time = []
    stdv = []
    my_label = 'Sequential'

    for n_threads in CONST_THREADS:
        mean_execution_time.append(mean_sequential_exec_time)
        stdv.append(sequential_stdv)
    print('Sequential ' + CONST_SHAPE + ': ' + str(mean_sequential_exec_time))

    #plt.plot(CONST_X_AXIS, mean_execution_time, CONST_COLORS[count] + 'o--', label=my_label)
    #plt.errorbar(CONST_X_AXIS, mean_execution_time, stdv, fmt='|', ecolor='k')
    plt.xticks(CONST_X_AXIS, CONST_THREADS)
    plt.grid(True)
    plt.legend(loc=1)
    #plt.ylim([0,3.5])
    plt.savefig('./logs_plots/' + CONST_MACHINE + '_' + CONST_SHAPE + '_' + str(CONST_POINTS) + '.eps', format='eps')
    plt.show()
    plt.clf()


# Plot speedup for fixed num of points, num of threads on x axis -------------------------------------------------------
def plot_speedup_fixed_points(algorithms, sequential_algorithm):

    sequential_exec_time = sequential_algorithm.execution_time[CONST_POINTS]
    mean_sequential_exec_time = float(np.average(sequential_exec_time) / (10 ** 6))

    plt.title("Speedup")
    plt.ylabel('Speedup')
    plt.xlabel('Number of threads')

    count = 0
    for algorithm_name in CONST_ALGORITHMS_NAMES:

        speedup = []
        stdv = []
        my_label = algorithm_name
        for n_threads in CONST_THREADS:
            curr_algorithm = algorithms[algorithm_name + ':' + str(n_threads)]
            measured_execution_time = curr_algorithm.execution_time[CONST_POINTS]

	    speedup.append(mean_sequential_exec_time / float(np.average(measured_execution_time) / (10 ** 6)))

        #plt.semilogy(CONST_X_AXIS, speedup, CONST_COLORS[count] + 'o--', label=my_label, basey=2)
        plt.plot(CONST_X_AXIS, speedup, CONST_COLORS[count] + 'o--', label=my_label)
        count += 1

    plt.plot(CONST_X_AXIS, CONST_THREADS, 'k-', label='n_threads')
    #plt.semilogy(CONST_X_AXIS, CONST_THREADS, 'k-', label='n_threads', basey=2)
    plt.xticks(CONST_X_AXIS, CONST_THREADS)
    #plt.yticks(CONST_X_AXIS, CONST_THREADS)
    plt.yticks(CONST_THREADS, CONST_THREADS)
    plt.grid(True)
    plt.legend(loc=2)
    plt.savefig('./logs_plots/speedup_' + CONST_MACHINE + '_' + CONST_SHAPE + '_' + str(CONST_POINTS) + '.eps', format='eps')
    plt.show()
    plt.clf()


# Plot execution time for fixed num of points, num of threads on x axis BOXPLOTS ---------------------------------------
def plot_box_time_fixed_points(algorithms, sequential_algorithm):

    for algorithm_name in CONST_ALGORITHMS_NAMES:

        plt.title(algorithm_name)
        plt.ylabel('Execution Time [s]')
        plt.xlabel('Number of threads')

        mean_execution_time = []
        stdv = []
        datas = []
        my_label = algorithm_name
        for n_threads in CONST_THREADS:
            curr_algorithm = algorithms[algorithm_name + ':' + str(n_threads)]
            measured_execution_time = curr_algorithm.execution_time[CONST_POINTS]

            datas.append(np.array(measured_execution_time) / (10 ** 6))

        plt.boxplot(datas)

        plt.xticks(np.arange(1, len(CONST_THREADS) + 1), CONST_THREADS)
        plt.grid(True)
        #plt.legend(loc=1)
        plt.show()
        plt.clf()


# Plot speedup for every algorithms for fixed num of points, nthreads on x axis with theoretical boundary --------------
def plot_theoretical_boundaries(algorithms, sequential_algorithm):

    sequential_exec_time = sequential_algorithm.execution_time[CONST_POINTS]
    mean_sequential_exec_time = float(np.average(sequential_exec_time) / (10 ** 6))

    plt.title("Speedup")
    plt.ylabel('Speedup')
    plt.xlabel('Number of threads')
    count = 0
    for algorithm_name in CONST_ALGORITHMS_NAMES:
        mean_execution_time = []
        stdv = []
        my_label = algorithm_name
        for n_threads in CONST_THREADS:
            curr_algorithm = algorithms[algorithm_name + ':' + str(n_threads)]
            measured_execution_time = curr_algorithm.execution_time[CONST_POINTS]

            mean_execution_time.append(mean_sequential_exec_time / float(np.average(measured_execution_time) / (10 ** 6)))
            stdv.append(float(np.std(measured_execution_time) / (10 ** 6)))

        plt.plot(CONST_THREADS, mean_execution_time, 'bo--', label=my_label)

        plt.plot(CONST_THREADS, calculate_boundary(algorithm_name), 'r')
        count += 1

        plt.xticks(CONST_THREADS)
        plt.grid(True)
        plt.legend(loc=1)
        plt.show()
        plt.clf()


# Map of algorithms. The map will be {algorithm_name, Algorithm}
algorithms_map = build_algorithms()

# Sequential algorithm
seq_algorithm = algorithms_map['Sequential:1']

# Execution Time plotter
plot_execution_time(algorithms_map)

# Speedup plotter
plot_speedup(algorithms_map, seq_algorithm)

# Execution Time plotter threads on x axis
plot_execution_time_fixed_points(algorithms_map, seq_algorithm)

# Speedup plotter threads on x axis
plot_speedup_fixed_points(algorithms_map, seq_algorithm)

# Plot Speedup x numof threads with theoretical boundaries
plot_theoretical_boundaries(algorithms_map, seq_algorithm)

# Box Plots for fixed points
plot_box_time_fixed_points(algorithms_map, seq_algorithm)
