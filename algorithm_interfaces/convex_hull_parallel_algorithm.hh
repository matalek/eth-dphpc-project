#ifndef CONVEX_HULL_PARALLEL_ALGORITHM
#define CONVEX_HULL_PARALLEL_ALGORITHM

#include <vector>
#include "../geometric_helpers.hh"
#include "convex_hull_algorithm.hh"
#include "convex_hull_sequential_algorithm.hh"
#include "../sequential/andrew_algorithm.hh"

using namespace std;

class ConvexHullParallelAlgorithm : public ConvexHullAlgorithm {

public:
	ConvexHullParallelAlgorithm(int threads)
			: threads(threads), sequential_algorithm(new AndrewAlgorithm()) { }

	virtual ~ConvexHullParallelAlgorithm() {
		delete(sequential_algorithm);
	}

protected:
	int threads;
	AndrewAlgorithm* sequential_algorithm;

};

#endif // CONVEX_HULL_PARALLEL_ALGORITHM