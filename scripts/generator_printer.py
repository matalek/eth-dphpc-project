#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np
import sys
import time
import subprocess


alg = (subprocess.check_output('cat tmp.log', shell=True))

i = 0
first_line = 0
x = []
y = []
for point in alg.split('\n'):
    if first_line < 2:
        first_line += 1
        continue

    line = point.split(' ')
    if line[0] != '':
        # Check if duplicate points are present
        if int(line[0]) in x:
            print 'DUPLICATE'
        x.append(int(line[0]))
        y.append(int(line[1]))

# Plot results
plt.ylim([np.amin(y) + np.amin(y)/10,np.amax(y) + np.amax(y)/10])
plt.xlim([np.amin(x) + np.amin(x)/10,np.amax(x) + np.amax(x)/10])
plt.plot(x,y, 'o-')
plt.grid(True)
i += 1

plt.show()
