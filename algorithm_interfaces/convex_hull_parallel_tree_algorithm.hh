#ifndef CONVEX_HULL_PARALLEL_TREE_ALGORITHM
#define CONVEX_HULL_PARALLEL_TREE_ALGORITHM

#include <vector>
#include <memory>
#include "../geometric_helpers.hh"
#include "../parallel_helper.hh"
#include "convex_hull_sequential_algorithm.hh"
#include "convex_hull_parallel_algorithm.hh"

using namespace std;

template <class R>
class ConvexHullParallelTreeAlgorithm : public ConvexHullParallelAlgorithm {

public:
	ConvexHullParallelTreeAlgorithm(int threads)
		: ConvexHullParallelAlgorithm(threads) { }

	// Function which calculates a convex hull of a given points set.
	shared_ptr<HullWrapper> convex_hull(vector<POINT*>& points) override {
		high_resolution_clock::time_point start_time = high_resolution_clock::now();
		ConvexHullAlgorithm::sequential_time = 0;

		int n = points.size();

		int threads_rounded = 1;
		while (threads_rounded < threads) {
			threads_rounded <<= 1;
		}

		shared_ptr<HullWrapper> partial_results[2 * threads_rounded];
		#pragma omp parallel num_threads(threads)
		{
			int id = omp_get_thread_num();
			// Calculating part of points for the given thread.
			pair<int, int> range = ParallelHelper::get_range(n, threads, id);
			// Calculating convex hull of the appropriate part of points.
			partial_results[threads_rounded + id] =  sequential_algorithm->convex_hull(points, range.first, range.second);
		}

		// Creating dummy nodes.
		if (threads_rounded > threads) {
			#pragma omp parallel num_threads(threads_rounded - threads)
			{
				int id = threads + omp_get_thread_num();
				auto upper_hull = shared_ptr<ConvexHullRepresentation>(new VectorConvexHullRepresentation(shared_ptr<POINTS>(new POINTS()), true));
				auto lower_hull = shared_ptr<ConvexHullRepresentation>(new VectorConvexHullRepresentation(shared_ptr<POINTS>(new POINTS()), false));
				partial_results[threads_rounded + id] =  shared_ptr<HullWrapper>(new HullWrapper(upper_hull, lower_hull));
			}
		}

		ConvexHullAlgorithm::sequential_time =  duration_cast<microseconds>( high_resolution_clock::now() - start_time).count();

		int level = threads_rounded >> 1;
		while (level > 0) {
			#pragma omp parallel num_threads(level)
			{
				// Merging threads from level * 2.
				int id = omp_get_thread_num();
				int pos = level + id; // Position in results tree.
				partial_results[pos << 1]->merge(partial_results[(pos << 1) + 1]);
				partial_results[level + id] = partial_results[pos << 1];
			}
			level >>= 1;
		}
		return partial_results[1];
	}
};

#endif // CONVEX_HULL_PARALLEL_TREE_ALGORITHM
