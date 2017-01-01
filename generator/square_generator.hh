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
#include "./geometric_helpers.hh"

using namespace std;

struct points_compare {
	bool operator() (const POINT a, const POINT b) {
		return a.x == b.x ? 0 : a.x < b.x;
	}
};

int rand_coordinate(int range) {
	return (rand() % (2 * range + 1)) - range;
}

vector<POINT> generate_points(int n) {
	srand(time(NULL));

	// Number of points.
	int range = 1000000000;


	set<POINT, points_compare> points;
	vector<POINT> resulting_points;
	// Range should be big enough so we can easily generate points that will not
	// conflict (i.e. be the same) with others.
	while (points.size() < n) {
		POINT point;
		point = POINT(rand_coordinate(range), rand_coordinate(range));
		points.insert(point);
	}

	for (set<POINT, points_compare>::iterator it = points.begin(); it != points.end(); ++it) {
		resulting_points.push_back(*it);
	}
	return resulting_points;
}