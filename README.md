# Parallel convex hull

## Comparator script

Execute from prompt command:
```sh
$ chmod u+x algorithm_comparator.py
```
and then 
```sh
$ ./algorithm_comparator.py
# -c <number of different combinations of number of points> 
# -w <width of steps>
# -s <starting number of points>
# -r <range of points coordinates>
# -R <number of repetition for each number of points>
# -a <algorithms to compare in the format <algo1:num_of_threads algo2:num_of_threads algo3:num_of_threads ...>>
```
For example:
```sh
./algorithm_comparator.py -c 100 -w 100 -s 1000 -r 1000 -R 2 -a simple_parallel/simple_parallel:4 simple_parallel/simple_parallel:8 ...
```

## Plotter script

Execute from prompt command:
```sh
$ chmod u+x tester_script.py
```
and then 

```sh
$ ./plotter.py
# -c <number of different combination in number of points>
# -w <step width>
# -s <starting value>
# -a <algorithms to compare in the format <algo1:num_of_threads algo2:num_of_threads algo3:num_of_threads ...>>
```

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

## Building tester program

Execute from prompt command:
```sh
$ make
```

## Running tester program

Execute from prompt command:
```sh
$ ./tester algorithm_name [threads_count]
```
where:
- `algorithm_name` is the name of the algorithm. Available algorithms are listed in the 
table below.
- `threads_count` (optional) is the number of threads in case of parallel version.

### Available algorithms
| Algorithm name | Type | Implementation |
|---|---|---|
| Sequential | Sequential | [AndrewAlgorithm](sequential/andrew_algorithm.hh) |
| SequentialGraham | Sequential | [GrahamAlgorithm](sequential/graham_algorithm.hh) |
| SimpleParallel | Parallel | [SimpleParallelAlgorithm](simple_parallel/simple_parallel.hh) |

## CGAL implementation
In order to compile CGAL version of the algorithm in `cgal` directory execute:
```sh
$ cmake .
$ make
```
It should produce executable file `cgal_graham_andrew`.

CGAL algorithm implementation takes only points as an input (without number of points) and as an output it writes only points (without number of points).
