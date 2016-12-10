#!/usr/bin/env python
import csv
import sys
import numpy as np
import matplotlib.pyplot as plt

# Usage ./plotter.py
# -a <algorithms to compare in the format <algo1:num_of_threads algo2:num_of_threads algo3:num_of_threads ...>>


colors = ['b', 'r', 'g', 'k', 'y']
count = 0
sequential_time = []

# Plot execution time -----------------------------------------------------------------------------------

plt.title("Performance comparison")
plt.ylabel('Response Time [s]')
plt.xlabel('Number of input points')
plt.grid(True)

for index in range(2, len(sys.argv)):

    num_of_points = []
    algorithm = sys.argv[index]
    flag = -1
    execution_time = []

    with open('./log_files/log_results_' + algorithm.replace("/", "_").replace(":", "_t_") + '.csv', 'rb') as f:
        reader = csv.reader(f)
        for row in reader:
            # skip first row
            if flag == -1:
                flag += 1
                continue
            num_of_points.append(int(row[0]))
            execution_time.append(float(row[1]) / (10 ** 6))

    if algorithm == 'Sequential:1':
        sequential_time = execution_time

    if algorithm == 'Sequential:1':
        my_label = 'Sequential'
    else:
        my_label = algorithm.replace("_", " ").replace(":", " ") + ' threads'

    plt.plot(num_of_points, execution_time, colors[count] + '-', label=my_label)
    count += 1

plt.legend(loc=2)

plt.savefig('./logs_plots/' + ("&".join(sys.argv[8:])).replace("/", "_") + '.png')
plt.show()
plt.clf()


# Plot speedup -----------------------------------------------------------------------------------
plt.title("Speedup")
plt.ylabel('Speedup')
plt.xlabel('Number of input points')
plt.grid(True)
count = 0

for index in range(2, len(sys.argv)):
    algorithm = sys.argv[index]

    # Skip sequential algorithm
    if algorithm == 'Sequential:1':
        continue

    num_of_points = []
    speedup = []
    flag = -1

    with open('./log_files/log_results_' + algorithm.replace("/", "_").replace(":", "_t_") + '.csv', 'rb') as f:
        reader = csv.reader(f)
        for row in reader:
            # skip first row
            if flag == -1:
                flag += 1
                continue
            num_of_points.append(int(row[0]))
            speedup.append((sequential_time[flag] * (10 ** 6)) / float(row[1]))
            flag += 1

        my_label = algorithm.replace("_", " ").replace(":", " ") + ' threads'
    plt.plot(num_of_points, speedup, colors[count] + '-', label=my_label)
    count += 1

plt.legend(loc=2)

plt.savefig('./logs_plots/' + ("&".join(sys.argv[8:])).replace("/", "_") + '.png')
plt.show()
plt.clf()
