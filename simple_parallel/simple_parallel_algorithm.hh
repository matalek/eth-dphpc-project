#ifndef SIMPLE_PARALLEL
#define SIMPLE_PARALLEL

#include <vector>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <omp.h>

#include "../geometric_helpers.hh"
#include "../merge_hull.hh"
#include "../sequential/andrew_algorithm.hh"
#include "../convex_hull_parallel_algorithm.hh"

using namespace std;

class SimpleParallelAlgorithm : public ConvexHullParallelAlgorithm {

public:

	SimpleParallelAlgorithm(int threads, ConvexHullSequentialAlgorithm* sequential_algorithm) 
			: ConvexHullParallelAlgorithm(threads, sequential_algorithm) { }
	
	// Function which calculates a convex hull of given points set.
	vector<POINT*> convex_hull(vector<POINT*>& points) override {
		int n = points.size();

		vector<POINT*> partial_results[2 * threads];
		#pragma omp parallel num_threads(threads)
		{
			int id = omp_get_thread_num();
			// Calculating part of points for the given thread.
			pair<int, int> range = get_range(n, id);
			vector<POINT*> working_points;
			for (int i = range.first; i < range.second; i++) {
				working_points.push_back(points[i]);
			}
			// Calculating convex hull of the appropriate part of points.
			partial_results[threads + id] = sequential_algorithm->convex_hull(working_points);
		}


		int level = threads >> 1;
		while (level > 0) {
			#pragma omp parallel num_threads(level)
			{
				// Merging threads from level * 2.
				int id = omp_get_thread_num();
				int pos = level + id; // Position in results tree.
				partial_results[level + id] = mergeHulls(partial_results[pos << 1], partial_results[(pos << 1) + 1]);
			}
			level >>= 1;
		}

		return partial_results[1];
	}

private:

	pair<int, int> get_range(int n, int id) {
		int batch_size = n / threads;

		int start = batch_size * id;
		int end;
		if (id == threads - 1) {
			end = n;
		} else {
			end = batch_size * (id + 1); 
		}

		return make_pair(start, end);
	}
};

#endif // SIMPLE_PARALLEL
