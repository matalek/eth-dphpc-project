#ifndef CONVEX_HULL_ALGORITHM
#define CONVEX_HULL_ALGORITHM

#include <vector>
#include "geometric_helpers.hh"

using namespace std;

class ConvexHullAlgorithm {

public:
	virtual ~ConvexHullAlgorithm() { };

	virtual vector<POINT*> convex_hull(vector<POINT*>&) = 0;

};

#endif // CONVEX_HULL_ALGORITHM