#ifndef CONVEX_HULL_PARALLEL_TREE_ALGORITHM
#define CONVEX_HULL_PARALLEL_TREE_ALGORITHM

#include <vector>
#include <memory>
#include "../geometric_helpers.hh"
#include "convex_hull_sequential_algorithm.hh"
#include "convex_hull_parallel_algorithm.hh"

using namespace std;

template <class R>
class ConvexHullParallelTreeAlgorithm : public ConvexHullParallelAlgorithm {

public:
	ConvexHullParallelTreeAlgorithm(int threads) 
		: ConvexHullParallelAlgorithm(threads) { }

	// Function which calculates a convex hull of a given points set.
	shared_ptr<vector<POINT*> > convex_hull(vector<POINT*>& points) override {
		int n = points.size();

		shared_ptr<ConvexHullRepresentation> partial_results[2 * threads];
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
			shared_ptr<vector<POINT*> > lower_hull_points = sequential_algorithm->lower_convex_hull(working_points);
            shared_ptr<vector<POINT*> > upper_hull_points = sequential_algorithm->upper_convex_hull(working_points);
			partial_results[threads + id] = shared_ptr<ConvexHullRepresentation>(new R(lower_hull_points,upper_hull_points));
		}

		int level = threads >> 1;
		while (level > 0) {
			#pragma omp parallel num_threads(level)
			{
				// Merging threads from level * 2.
				int id = omp_get_thread_num();
				int pos = level + id; // Position in results tree.
				partial_results[level + id] = partial_results[pos << 1]->merge(partial_results[(pos << 1) + 1]);
			}
			level >>= 1;
		}

		return partial_results[1]->get_points();
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

#endif // CONVEX_HULL_PARALLEL_TREE_ALGORITHM