# dphpc-project

Convex hull


How to run simple script:

execute from prompt command "chmod u+x sequentialScript.py"
and then "./sequentialScript.py -f <path to c++ program> -g <path to points generator program>
-c <Number of different combinations of number of points> -w <width of steps> -s <starting number of points>
-r <number of repetition for each number of points>"

How to build using Makefile (currently for sequential version)

execute from prompt command "make ALGORITHM=algorithm_name", where
algorithm_name is the name of the header file with the implementation
of convex_hull function (without .hh extension).
