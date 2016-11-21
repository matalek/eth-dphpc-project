#ifndef CONVEX_HULL_ALGORITHM
#define CONVEX_HULL_ALGORITHM

#include <vector>
#include <memory>
#include "../geometric_helpers.hh"

using namespace std;

class ConvexHullAlgorithm {

public:
	virtual ~ConvexHullAlgorithm() { };

	virtual shared_ptr<vector<POINT*> > convex_hull(vector<POINT*>&) = 0;

};

#endif // CONVEX_HULL_ALGORITHM