// Simple cricle points generator, taking number of points and range.
// Will generate points from [-range, range].

#include <vector>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <cmath>
#include <iostream>
#include "../geometric_helpers.hh"

using namespace std;

struct points_compare {
	bool operator() (const POINT a, const POINT b) {
		return a.x == b.x ? 0 : a.x < b.x;
	}
};

double rand_angle(int range) {
	return double((rand() % (2 * range + 1)) - range);
}

int main() {
	srand(time(NULL));

	// Number of points.
	int n, range;
	cin >> n >> range;

	set<POINT, points_compare> points;
	// Range should be big enough so we can easily generate points that will not
	// conflict (i.e. be the same) with others.
	while (points.size() < n) {
		 double theta = rand_angle(range);
		 POINT point = POINT(range*cos(theta), range*sin(theta));
		 points.insert(point);
	}

	cout << n << "\n";
	for (set<POINT, points_compare>::iterator it = points.begin(); it != points.end(); ++it) {
		it->print();
	}

	return 0;
}
