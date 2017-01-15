#ifndef DISK_GENERATOR
#define DISK_GENERATOR

// Simple points generator, taking number of points and range.
// Will generate points from [-range, range].

#include <vector>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <cmath>
#include <iostream>
#include <string>
#include "../geometric_helpers.hh"

#include "generator.hh"

using namespace std;

class DiskGenerator : public Generator {
	vector<POINT> generate_points(int n) {
		srand(time(NULL));

		set<POINT, points_compare> points;
		vector<POINT> resulting_points;
		// Range should be big enough so we can easily generate points that will not
		// conflict (i.e. be the same) with others.
		while (points.size() < n) {
			// Circumference is x2 + y2 = range2
			 POINT point;
			 do{
				point = POINT(rand_coordinate(range), rand_coordinate(range));
			 } while(pow(point.x,2) + pow(point.y,2) > pow(range,2));
			 points.insert(point);
		}

		for (set<POINT, points_compare>::iterator it = points.begin(); it != points.end(); ++it) {
			resulting_points.push_back(*it);
		}
		return resulting_points;
	}
};

#endif // DISK_GENERATOR
