// Simple points generator, taking number of points and range.
// Will generate points from [-range, range].

#include <vector>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <set>
#include "../geometric_helpers.hh"

using namespace std;

struct points_compare {
	bool operator() (const POINT a, const POINT b) {
		return a.x == b.x ? a.y < b.y : a.x < b.x;
	}
};

int rand_coordinate(int range) {
	return (rand() % (2 * range + 1)) - range;
}

int main() {
	srand(time(NULL));

	// Number of points.
	int n, range;
	scanf("%d%d", &n, &range);

	set<POINT, points_compare> points;
	// Range should be big enough so we can easily generate points that will not
	// conflict (i.e. be the same) with others.
	while (points.size() < n) { 
		 points.insert(POINT(rand_coordinate(range), rand_coordinate(range)));
	}

	printf("%d\n", n);
	for (POINT point : points) {
		point.print();
	}

	return 0;
}
