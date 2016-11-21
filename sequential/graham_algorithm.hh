#ifndef GRAHAM_ALGORITHM
#define GRAHAM_ALGORITHM

#include <vector>
#include <cstdio>
#include <cmath>
#include <algorithm>

#include "../geometric_helpers.hh"
#include "../algorithm_interfaces/convex_hull_sequential_algorithm.hh"

using namespace std;

class GrahamAlgorithm : public ConvexHullSequentialAlgorithm {

public:

	~GrahamAlgorithm() override { }

	// Function which calculates a convex hull of given points set.
	shared_ptr<vector<POINT*> > convex_hull(vector<POINT*>& points) override {
		// Vector of pointer to points to be sorted.
		vector<POINT*> working_points;
		vector<POINT*> result_points;

		// Fill up the working_points vector with pointers to points.
		POINT* left_bottom = nullptr;
		for (size_t i = 0; i < points.size(); i++) {
			if (left_bottom == nullptr || OrderXY(points[i], left_bottom)) {
				left_bottom = points[i];
			}
		}

		for (size_t i = 0; i < points.size(); i++) {
			if (left_bottom != points[i]) {
				working_points.push_back(points[i]);
			}
		}

		result_points.push_back(left_bottom);

		// Sort points in increasing order with regards to coordinates.
		sort(working_points.begin(), working_points.end(), AngleComparator(left_bottom));

		// Iterate once over points to create the convex hull.
		size_t size_lower_limit = 1;
		for (POINT* point : working_points) {
			add_point_to_convex_hull(result_points, point, size_lower_limit);
		}

		// Whole algorithm is not very effective, so we can allow creating return pointer
		// by coping the vector.
		return shared_ptr<vector<POINT*> >(new vector<POINT*>(result_points));
	}

private:

	void add_point_to_convex_hull(vector<POINT*>& result_points, POINT* point,
			size_t size_lower_limit) {
		while (result_points.size() > size_lower_limit 
				&& Det((*result_points[result_points.size() - 2]),
						(*result_points[result_points.size() - 1]), 
						(*point)) <= 0) {
			result_points.pop_back();
		}
		result_points.push_back(point);
	}

	struct AngleComparator {
		AngleComparator(POINT* point) : point(point) { }

		bool operator()(const POINT* a, const POINT* b) {
			LL det = Det((*point), (*a), (*b));
			if (det == 0) {
				return abs_int(point->x - a->x) + abs_int(point->y - a->y) >
						abs_int(point->x - b->x) + abs_int(point->y - b->y);
			}
			return det > 0;
		}

		POINT* point;
	};
};

#endif // GRAHAM_ALGORITHM
