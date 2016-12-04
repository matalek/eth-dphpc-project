#ifndef ANDREW_ALGORTIHM
#define ANDREW_ALGORTIHM

#include <vector>
#include <stdio.h>
#include <algorithm>

#include "../geometric_helpers.hh"
#include "../algorithm_interfaces/convex_hull_sequential_algorithm.hh"
#include "../data_structures/vector_convex_hull_representation.hh"

using namespace std;

class AndrewAlgorithm : public ConvexHullSequentialAlgorithm {

public:

	~AndrewAlgorithm() override { }

	// Function which calculates a convex hull of given points set.
	shared_ptr<HullWrapper> convex_hull(vector<POINT*>& points) override {
		shared_ptr<VectorConvexHullRepresentation> lower_hull = shared_ptr<VectorConvexHullRepresentation>(new VectorConvexHullRepresentation(lower_convex_hull(points),false));
		shared_ptr<VectorConvexHullRepresentation> upper_hull = shared_ptr<VectorConvexHullRepresentation>(new VectorConvexHullRepresentation(upper_convex_hull(points),true));
		shared_ptr<HullWrapper> ret = shared_ptr<HullWrapper>(new HullWrapper(upper_hull, lower_hull));
		return ret;
	}

	shared_ptr<vector<POINT*> > lower_convex_hull(vector<POINT*>& points) {
		// Vector of pointer to points to be sorted.
		shared_ptr<vector<POINT*> > result_points = shared_ptr<vector<POINT*> >(new vector<POINT*>());

		// Calculate first lower part of convex hull, connecting most left-bottom
		// point with most right-top point.
		unsigned int size_lower_limit = 1;
		for (POINT* point : points) {
			add_point_to_convex_hull(result_points, point, size_lower_limit);
		}

		return result_points;
	}

	shared_ptr<vector<POINT*> > upper_convex_hull(vector<POINT*>& points) {
		// Vector of pointer to points to be sorted.
		shared_ptr<vector<POINT*> > result_points = shared_ptr<vector<POINT*> >(new vector<POINT*>());

		// Calculate first lower part of convex hull, connecting most left-bottom
		// point with most right-top point.
		unsigned int size_lower_limit = 1;
		for (int i = points.size() - 1; i >= 0; --i) {
			add_point_to_convex_hull(result_points, points[i], size_lower_limit);
		}

		return result_points;
	}

private:

	void add_point_to_convex_hull(shared_ptr<vector<POINT*> > result_points, POINT* point,
			unsigned int size_lower_limit) {
		while (result_points->size() > size_lower_limit 
				&& Det((*(*result_points)[result_points->size() - 2]),
						(*(*result_points)[result_points->size() - 1]), 
						(*point)) <= 0) {
			result_points->pop_back();
		}
		result_points->push_back(point);
	}
};

#endif // ANDREW_ALGORTIHM
