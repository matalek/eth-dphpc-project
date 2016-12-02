#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np
import sys
import time
import subprocess

my_alg = sys.argv[1]

alg = []
alg.append(subprocess.check_output('cat tmp.log | ./tester ' + (' ').join(my_alg.split(':')), shell=True))
alg.append(subprocess.check_output('cat tmp.log | ./cgal/cgal_graham_andrew', shell=True))


i=0
for val in (211,212):
    firstline = 0
    x = []
    y = []
    for point in alg[i].split('\n'):
        if firstline<2:
            firstline+=1
            continue

        line = point.split(' ')
        #print line
        if line[0] != '':
            x.append(int(line[0]))
            y.append(int(line[1]))

    for index in range(0, len(x)-2):
        a = x[index]
        b = y[index]

        m = x[index + 1]
        n = y[index + 1]

        w = x[index + 2]
        z = y[index + 2]

        if (n-b)*(w-m) == (z-n)*(m-a):
            print 'Inline points'

    plt.subplot(val)
    plt.ylim([np.amin(y) + np.amin(y)/10,np.amax(y) + np.amax(y)/10])
    plt.xlim([np.amin(x) + np.amin(x)/10,np.amax(x) + np.amax(x)/10])
    plt.plot(x,y, 'o-')
    plt.grid(True)
    i += 1

plt.show()
