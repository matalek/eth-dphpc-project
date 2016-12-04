#ifndef NAIVE_PARALLEL
#define NAIVE_PARALLEL

#include <vector>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <omp.h>

#include "../geometric_helpers.hh"
#include "../merge_hull.hh"
#include "../sequential/andrew_algorithm.hh"
#include "../algorithm_interfaces/convex_hull_parallel_tree_algorithm.hh"

using namespace std;

class NaiveParallelAlgorithm : public ConvexHullParallelTreeAlgorithm<VectorConvexHullRepresentation> {

public:
	NaiveParallelAlgorithm(int threads) 
			: ConvexHullParallelTreeAlgorithm(threads) { }
};

#endif // NAIVE_PARALLEL
