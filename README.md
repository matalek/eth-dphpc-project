# dphpc-project

Convex hull


How to run simple script:

execute from prompt command "chmod u+x sequentialScript.py"
and then "./sequentialScript.py -f "Path to c++ program" -g "Path to points generator program"
-c "Number of different combinations of number of points" -w "Width of steps" -s "Starting number of points"
-r "Number of repetition for each number of points""

How to build using Makefile (currently for sequential version)

execute from prompt command "make ALGORITHM=algorithm_name", where
algorithm_name is the name of the header file with the implementation
of convex_hull function (without .hh extension).
