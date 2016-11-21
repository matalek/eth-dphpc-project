#ifndef ANDREW_ALGORTIHM
#define ANDREW_ALGORTIHM

#include <vector>
#include <stdio.h>
#include <algorithm>

#include "../geometric_helpers.hh"
#include "../algorithm_interfaces/convex_hull_sequential_algorithm.hh"

using namespace std;

class AndrewAlgorithm : public ConvexHullSequentialAlgorithm {

public:

	~AndrewAlgorithm() override { }

	// Function which calculates a convex hull of given points set.
	shared_ptr<vector<POINT*> > convex_hull(vector<POINT*>& points) override {
		// Vector of pointer to points to be sorted.
		vector<POINT*> working_points;
		shared_ptr<vector<POINT*> > result_points = shared_ptr<vector<POINT*> >(new vector<POINT*>());

		// Fill up the working_points vector with pointers to points.
		for (size_t i = 0; i < points.size(); i++) {
			working_points.push_back(points[i]);
		}

		// Calculate first lower part of convex hull, connecting most left-bottom
		// point with most right-top point.
		unsigned int size_lower_limit = 1;
		for (POINT* point : working_points) {
			add_point_to_convex_hull(result_points, point, size_lower_limit);
		}

		// We do not want already added points to be removed.
		size_lower_limit = result_points->size();
		for (int i = working_points.size() - 2; i >= 0; --i) {
			add_point_to_convex_hull(result_points, working_points[i],
					size_lower_limit);
		}

		// Last point has been added two times, so we have to remove it once.
		result_points->pop_back();
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
