#ifndef SIMPLE_PARALLEL
#define SIMPLE_PARALLEL

#include <vector>
#include "../geometric_helpers.hh"
#include "../sequential/convex_hull_graham_coordinate_sort.hh"
#include "../merge_hull.hh"
#include <stdio.h>
#include <algorithm>
#include <omp.h>

#include <iostream>
#include <string>

using namespace std;

#define THREADS_COUNT THREADS

pair<int, int> get_range(int n, int id) {
	int batch_size = n / THREADS_COUNT;

	int start = batch_size * id;
	int end;
	if (id == THREADS_COUNT - 1) {
		end = n;
	} else {
		end = batch_size * (id + 1); 
	}

	return make_pair(start, end);
}

// Function which calculates a convex hull of given points set.
vector<POINT*> convex_hull(vector<POINT*>& points) {
	int n = points.size();

	vector<POINT*> partial_results[2 * THREADS_COUNT];
	#pragma omp parallel num_threads(THREADS_COUNT)
	{
		int id = omp_get_thread_num();
		// Calculating part of points for the given thread.
		pair<int, int> range = get_range(n, id);
		vector<POINT*> working_points;
		for (int i = range.first; i < range.second; i++) {
			working_points.push_back(points[i]);
		}
		// Calculating convex hull of the appropriate part of points.
		partial_results[THREADS_COUNT + id] = convex_hull_sequential(working_points);
	}

	int level = THREADS_COUNT >> 1;
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


#endif // SIMPLE_PARALLEL
