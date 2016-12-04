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
#include "../algorithm_interfaces/convex_hull_parallel_simple_algorithm.hh"

using namespace std;

class SimpleParallelAlgorithm : public ConvexHullParallelSimpleAlgorithm<VectorConvexHullRepresentation> {

public:
	SimpleParallelAlgorithm(int threads) 
			: ConvexHullParallelSimpleAlgorithm(threads) { }
};

#endif // SIMPLE_PARALLEL
