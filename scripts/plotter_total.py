#!/usr/bin/env python
import Algorithm
import csv
import numpy as np
import matplotlib.pyplot as plt

# Usage ./plotter_benchmark.py
# -a <algorithms to compare in the format <algo1:num_of_threads algo2:num_of_threads algo3:num_of_threads ...>>


CONST_COLORS = ['b', 'r', 'g', 'k', 'b', 'r', 'g', 'k']

CONST_ALGORITHMS_NAMES = ['SimpleParallel']
CONST_POINTS = 10000000
x = []
for i in range(2, 257):
    x.append(i)
CONST_THREADS = x
CONST_X_AXIS = np.arange(1, len(CONST_THREADS) + 1)
CONST_SHAPE = 'square'
CONST_MACHINE = 'xeon'

CONST_SOURCE_FILE = ('./log_files/log_files_' + CONST_MACHINE + '/complete/' + CONST_MACHINE + '_' + CONST_SHAPE + '/' +
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
    plt.figure(num=None, figsize=(10, 3), facecolor='w', edgecolor='k')
    plt.gcf().subplots_adjust(bottom=0.15)
    sequential_exec_time = sequential_algorithm.execution_time[CONST_POINTS]
    mean_sequential_exec_time = float(np.average(sequential_exec_time) / (10 ** 6))
    sequential_stdv = float(np.std(sequential_exec_time) / (10 ** 6))

    plt.title("SimpleParallel algorithm performance")
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
    ticks = [2,4,8,16,32,64,128,256]
    plt.xlim([0,256])
    plt.xticks(ticks, ticks)
    #plt.legend(loc=1)
    plt.grid(True)
    #plt.ylim([0,3.5])
    plt.savefig('./logs_plots/total.eps', format='eps')
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
    ticks = [2, 4, 8, 16, 32, 64, 128, 256]
    plt.xlim([0, 256])
    plt.xticks(ticks, ticks)
    #plt.yticks(CONST_X_AXIS, CONST_THREADS)
    plt.yticks(ticks, ticks)
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


# Plot execution time for fixed num of points, num of threads on x axis BOXPLOTS all algos ------------------------------
def plot_box_all(algorithms, sequential_algorithm):
    
    colors = ['b', 'r', 'g']
    
    count = 0
    fig, ax = plt.subplots()
    plt.title('Performance comparison')
    for algorithm_name in CONST_ALGORITHMS_NAMES:


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

        boxplot_dict = ax.boxplot(
            datas,
            positions=(CONST_X_AXIS * len(CONST_ALGORITHMS_NAMES) + count -1),
            notch=True,
            widths=0.25)


        i = 0
        for b in boxplot_dict['boxes']:
            b.set_color(colors[count])
            if i == 0:
                b.set_label(algorithm_name)
            i += 1

        count += 1

    plt.xticks(np.arange(0, (len(CONST_THREADS) + 1) * len(CONST_ALGORITHMS_NAMES), len(CONST_ALGORITHMS_NAMES)),
               [0] + CONST_THREADS)
    plt.grid(True)
    plt.ylim(ymax=2)
    plt.legend(loc=1)
    plt.savefig('logs_plots/boxes.eps', format='eps')
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

# Box Plots for fixed points
plot_box_time_fixed_points(algorithms_map, seq_algorithm)

# Box Plots for fixed points all algos
plot_box_all(algorithms_map, seq_algorithm)

