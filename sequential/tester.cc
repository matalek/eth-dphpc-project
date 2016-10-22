#include <vector>
#include "geometric_helpers.hh"
#include "convex_hull_graham.hh"

int main() {
	// Number of points.
	int n;
	scanf("%d", &n);

	// Reading from standard input points.
	vector<POINT> points(n);
	for (int i = 0; i < n; i++) {
		int x, y;
		scanf("%d%d", &x, &y);
		points[i] = POINT(x, y);
	}

	auto convex_hull_points = convex_hull(points);
	printf("%lu\n", convex_hull_points.size());
	for (POINT* point : convex_hull_points) {
		point->print();
	}

	return 0;
}