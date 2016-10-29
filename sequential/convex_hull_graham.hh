#ifndef CONVEX_HULL_GRAHAM
#define CONVEX_HULL_GRAHAM

#include <vector>
#include "../geometric_helpers.hh"
#include <cstdio>
#include <cmath>
#include <algorithm>

using namespace std;

void add_point_to_convex_hull(vector<POINT*>& result_points, POINT* point,
		int size_lower_limit) {
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
			return abs_int(point->x - a->x) + abs_int(point->y - a->y) <
					abs_int(point->x - b->x) + abs_int(point->y - b->y);
		}
		return det > 0;
	}

	POINT* point;
};

// Function which calculates a convex hull of given points set.
vector<POINT*> convex_hull(vector<POINT>& points) {
	// Vector of pointer to points to be sorted.
	vector<POINT*> working_points;
	vector<POINT*> result_points;

	// Fill up the working_points vector with pointers to points.
	POINT* left_bottom = nullptr;
	for (int i = 0; i < points.size(); i++) {
		working_points.push_back(&points[i]);
		if (left_bottom == nullptr || OrderXY(&points[i], left_bottom)) {
			left_bottom = &points[i];
		}
	}

	// Sort points in increasing order with regards to coordinates.
	sort(working_points.begin(), working_points.end(), AngleComparator(left_bottom));

	// Iterate once over points to create the convex hull.
	int size_lower_limit = 1;
	for (POINT* point : working_points) {
		add_point_to_convex_hull(result_points, point, size_lower_limit);
	}

	return result_points;
}


#endif // CONVEX_HULL_GRAHAM
