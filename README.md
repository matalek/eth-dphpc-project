# Parallel convex hull


## Tester script

Execute from prompt command:
```sh
$ chmod u+x tester_script.py
```
and then 
```sh
$ ./tester_script.py -a <Algorithm name, including directory>
-c <Number of different combinations of number of points> 
-w <Width of steps> -s <Starting number of points>
-r <Range for points coordinates>
-R <Number of repetition for each number of points>
```
For example:
```sh
$ ./tester_script.py -a sequential/convex_hull_graham -c 10 -w 10000 -s 100000 -r 1000000000 -R 2
```

## Building algorithm implementation using Makefile

Execute from prompt command:
```sh
$ make ALGORITHM=algorithm_name
```
where algorithm_name is the name of the header file with the implementation
of convex_hull function (including directory, without .hh extension,
e.g. sequential/convex_hull_graham).

## CGAL implementation
In order to compile CGAL version of the algorithm in `cgal` directory execute:
```sh
$ cmake .
$ make
```
It should produce executable file `cgal_graham_andrew`.

CGAL algorithm implementation takes only points as an input (without number of points) and as an output it writes only points (without number of points).