#ifndef CONVEX_HULL_ALGORITHM
#define CONVEX_HULL_ALGORITHM

#include <vector>
#include <memory>
#include <chrono>
#include "../geometric_helpers.hh"
#include "../data_structures/hull_wrapper.hh"

using namespace std;
using namespace std::chrono;

class ConvexHullAlgorithm {

public:
	static long long int sequential_time;

	static int a;

	virtual ~ConvexHullAlgorithm() { };

	virtual shared_ptr<HullWrapper> convex_hull(vector<POINT*>&) = 0;
};

#endif // CONVEX_HULL_ALGORITHM