#ifndef VECTOR_CONVEX_HULL_REPRESENTATION
#define VECTOR_CONVEX_HULL_REPRESENTATION

#include <vector>
#include <cassert>
#include <memory>

#include "../geometric_helpers.hh"
#include "../merge_hull.hh"
#include "convex_hull_representation.hh"

using namespace std;

class VectorConvexHullRepresentation : public ConvexHullRepresentation {

public:
	VectorConvexHullRepresentation() { };

	VectorConvexHullRepresentation(shared_ptr<vector<POINT*> > points) : points(points) { };

	shared_ptr<ConvexHullRepresentation> merge(shared_ptr<ConvexHullRepresentation> hull) override {
		shared_ptr<VectorConvexHullRepresentation> hull_vector = std::static_pointer_cast<VectorConvexHullRepresentation>(hull);
		assert(hull_vector != nullptr && "Merging two different subclasses");

		vector<POINT*> res_points = mergeHulls((*this), (*hull_vector));
		return shared_ptr<VectorConvexHullRepresentation>(
				new VectorConvexHullRepresentation(shared_ptr<vector<POINT*> >(new vector<POINT*>(res_points))));
	}

	shared_ptr<vector<POINT*> > get_points() override {
		return points;
	}

	/*methods for commont tangent alg*/

	int find_rightmost_point() override {
		int index = 0;
		for (unsigned int i = 0; i < points.get()->size(); i++) {
			if (OrderXY(points.get()->at(index), points.get()->at(i))) {
				index = i;
			}
		}
		return index;
	}

	int find_leftmost_point() override {
		int index = 0;
		for (unsigned int i = 0; i < points.get()->size(); i++) {
			if (OrderXY(points.get()->at(i), points.get()->at(index))) {
				index = i;
			}
		}
		return index;
	}

	int go_counter_clockwise(int index) override {
		return (index + 1) % points.get()->size();
	}

	int go_clockwise(int index) override {
		return index > 0 ? index - 1 : points.get()->size()-1;
	}

	POINT* get_point(int index) override {
        return points.get()->at(index);
    }

protected:
	const int max_parallelism = 1;

private:
	shared_ptr<vector<POINT*> > points;

};

#endif // VECTOR_CONVEX_HULL_REPRESENTATION