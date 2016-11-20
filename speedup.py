#!/usr/bin/env python
import subprocess
import csv
import sys
import numpy as np
import matplotlib.pyplot as plt

# Usage ./plotter.py
# -c <number of different combination in number of points>
# -w <step width>
# -s <starting value>
# -a <algorithms to compare in the format <algo1:num_of_threads algo2:num_of_threads algo3:num_of_threads ...>>

starting_value = int(sys.argv[6])
step_width = int(sys.argv[4])
comb_number = int(sys.argv[2])

colors = ['b','r','g','k']
count = 0

plt.title("SPEEDUP")
plt.ylabel('Speedup')
plt.xlabel('Number of points')
plt.grid(True)

x = np.arange(starting_value, comb_number*step_width + starting_value, step_width)

num = []

for index in range(8 , len(sys.argv)):
    algorithm = sys.argv[index]
    flag = -1
    rt = []

    appendix = ''
    if algorithm.split("/")[0] != 'sequential':
        appendix = 'tds'

    with open('./log_files/log_results_' + algorithm.replace("/","_").replace(":","_t_")
                      + '.csv','rb') as f:
        reader = csv.reader(f)
        for row in reader:
            # skip first row
            if(flag == -1):
                flag += 1
                continue

            time = float((row[0].split('\t'))[1].replace('"', ''))

            if algorithm.split("/")[0] == 'sequential':
                num.append(time)
                print ('anal seq time = ' + str(time))
            else:
                rt.append(time)
                print ('anal par time = ' + str(time))

    if algorithm.split("/")[0] != 'sequential':
        denum = []
        for i in range(0, len(rt)):
            denum.append(num[i]/rt[i])
        y = np.array(denum)
        my_label = (algorithm.split("/")[1]).replace("_", " ").replace(":"," ")  + ' ' + appendix
        plt.plot(x, y, colors[count] + '-', label = my_label)
        count += 1

plt.legend(loc=2)


#plt.savefig('./plots/' + ("&".join(sys.argv[8:])).replace("/", "_") + '.png')
plt.show()