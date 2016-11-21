#ifndef VECTOR_CONVEX_HULL_REPRESENTATION
#define VECTOR_CONVEX_HULL_REPRESENTATION

#include <vector>
#include <cassert>
#include <memory>

#include "../geometric_helpers.hh"
#include "../merge_hull.hh"

using namespace std;

class VectorConvexHullRepresentation : public ConvexHullRepresentation {

public:
	VectorConvexHullRepresentation() { };

	VectorConvexHullRepresentation(shared_ptr<vector<POINT*> > points) : points(points) { };

	shared_ptr<ConvexHullRepresentation> merge(shared_ptr<ConvexHullRepresentation> hull) override {
		shared_ptr<VectorConvexHullRepresentation> hull_vector = std::static_pointer_cast<VectorConvexHullRepresentation>(hull);
		assert(hull_vector != nullptr && "Merging two different subclasses");

		vector<POINT*> res_points = mergeHulls(*points, *(hull_vector->points));
		return shared_ptr<VectorConvexHullRepresentation>(
				new VectorConvexHullRepresentation(shared_ptr<vector<POINT*> >(new vector<POINT*>(res_points))));
	}

	shared_ptr<vector<POINT*> > get_points() override {
		return points;
	}

protected:
	const int max_parallelism = 1;

private:
	shared_ptr<vector<POINT*> > points;

};

#endif // VECTOR_CONVEX_HULL_REPRESENTATION