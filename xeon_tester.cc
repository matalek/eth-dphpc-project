#include <vector>
#include <memory>
#include <cassert>
#include <stdio.h>
#include <chrono>
#include <string>
#include <iostream>
#include <fstream>

#include "geometric_helpers.hh"
#include "merge_hull.hh"
#include "./generator/generator.hh"

#include "algorithm_interfaces/convex_hull_sequential_algorithm.hh"
#include "algorithm_interfaces/convex_hull_parallel_algorithm.hh"
#include "data_structures/convex_hull_representation.hh"
#include "data_structures/vector_convex_hull_representation.hh"

#include "sequential/andrew_algorithm.hh"

#include "simple_parallel/simple_parallel_algorithm.hh"
#include "naive_parallel/naive_parallel_algorithm.hh"
#include "algorithm_interfaces/convex_hull_parallel_tree_algorithm.hh"
#include "hull_tree/hull_tree_algorithm.hh"

using namespace std;
using namespace std::chrono;

// high_resolution_clock::time_point ConvexHullAlgorithm::middle_time;
LL ConvexHullAlgorithm::sequential_time;

// In order to add a new algorithm:
// - create a class subtyping ConveHullAlgorithm providing implementation
// of the new algorithm,
// - include its header file in this file,
// - add an apropriate 'if' statement in the 'load_algorithm' function.

bool is_sequential;
string algorithms[3] = {"SimpleParallel", "NaiveParallel", "HullTree"};
string threads_count[6] = {"2", "4", "8", "16", "32", "64"};
string points_dimension[2] = {"10000000", "10000000"};

// Loads an appropriate algorithm based on command line params.
ConvexHullAlgorithm* load_algorithm(string arg) {
	ConvexHullAlgorithm* algorithm;
	// TODO(matalek): provide nicer way to create an appropriate algorithm.

	int threads;
	string name;
	size_t split_point = arg.find(':');

	if (split_point != string::npos) {
		name = arg.substr(0, split_point);
		threads = stoi(arg.substr(split_point + 1));
	} else {
		name = arg;
	}

	if (name == "Sequential") {
		is_sequential = true;
		algorithm = new AndrewAlgorithm();
	} else if (name == "NaiveParallel") {
		assert(threads > 0);
		algorithm = new NaiveParallelAlgorithm(threads);
	} else if (name == "SimpleParallel") {
		assert(threads > 0);
		algorithm = new SimpleParallelAlgorithm(threads);
	} else if (name == "HullTree") {
		assert(threads > 0);
		algorithm = new HullTreeAlgorithm(threads);
	} else {
		assert(false && "No algorithm found with this name");
	}
	return algorithm;
}

int main(int argc, char* argv[]) {
	cout << "START\n";
	ConvexHullAlgorithm* algorithm;
	for (auto num_of_points : points_dimension){
		for (int rep = 0; rep < 4; rep++){
			//generate input points of size num_of_points
			int n = stoi(num_of_points);
			// Reading points.
			vector<POINT> points;

			// TODO call correct generator based on argv
			points = generate_points(n);

			vector<POINT*> points_pointers(n);	
			for (int i = 0; i < n; i++) {
				points_pointers[i] = &points[i];
			}
			for (auto algorithm_name : algorithms){
				for (auto n_threads : threads_count){
					algorithm = load_algorithm(algorithm_name + ":" + n_threads);
					ofstream output_file;
					output_file.open ("/mnt/hostfs/team08/log_files/" + algorithm_name + "_" + n_threads + ".log", ios::app);			
	
					high_resolution_clock::time_point t1 = high_resolution_clock::now();
				    	shared_ptr<HullWrapper> convex_hull_points = shared_ptr<HullWrapper>(algorithm->convex_hull(points_pointers));
					high_resolution_clock::time_point t2 = high_resolution_clock::now();

					output_file << "POINTS: " << num_of_points << "\nREP: " << (rep + 1);
					output_file << "\nTIME: ";
					output_file << ConvexHullAlgorithm::sequential_time << " "
							<< duration_cast<microseconds>( t2 - t1 ).count() - ConvexHullAlgorithm::sequential_time << "\n";
					output_file << "";
					output_file.close();
				}
			}

			algorithm = load_algorithm("Sequential:1");
			ofstream output_file;
			output_file.open ("/mnt/hostfs/team08/log_files/Sequential_1.log", ios::app);			

			//computing exec time (could find better way)
			high_resolution_clock::time_point t1 = high_resolution_clock::now();
		    	shared_ptr<HullWrapper> convex_hull_points = shared_ptr<HullWrapper>(algorithm->convex_hull(points_pointers));
			high_resolution_clock::time_point t2 = high_resolution_clock::now();

			output_file << "POINTS: " << num_of_points << "\nREP: " << (rep + 1);
			output_file << "\nTIME: ";
			output_file << duration_cast<microseconds>( t2 - t1 ).count() << " " << 0 << "\n\n";
			output_file << "";
			output_file.close();
		}
	}
	return 0;
}
