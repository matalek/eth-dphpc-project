# Parallel convex hull

## Comparator script

The comparator script it's used to execute different algorithms for different inputs and evaluate the results in terms of time and correctness.
The resulting csv files are stored in the log_files folder

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
./algorithm_comparator.py -c 10 -w 100000 -s 100000 -r 1000000 -R 2 -a SimpleParallel:4 SimpleParallel:8 SequentialGraham:1 ...
```
REMEMBER to also provide a sequential version when needed, with 1 as num_of_threads

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
| SimpleParallel | Parallel | [SimpleParallelAlgorithm](simple_parallel/simple_parallel_algorithm.hh) |

## CGAL implementation
In order to compile CGAL version of the algorithm in `cgal` directory execute:
```sh
$ cmake .
$ make
```
It should produce executable file `cgal_graham_andrew`.

CGAL algorithm implementation takes only points as an input (without number of points) and as an output it writes only points (without number of points).
