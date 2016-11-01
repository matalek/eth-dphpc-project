#include <vector>
#include "geometric_helpers.hh"
#include "merge_hull.hh"

// Including appropriate library according to defined
// ALGORITHM variable.
#define QUOTE(x) #x
#define INCLUDE_FILE(x) QUOTE(x.hh)

#include INCLUDE_FILE(ALGORITHM)

#include <stdio.h>
#include <chrono>
#include <string>

int main() {

	using namespace std;

	using namespace std::chrono;

	// Number of points.
	int n;
	scanf("%d", &n);

	// Reading from standard input points.
	vector<POINT> pointsA(n);
	for (int i = 0; i < n; i++) {
		int x, y;
		scanf("%d%d", &x, &y);
		pointsA[i] = POINT(x, y);
	}

	scanf("%d", &n);

	// Reading from standard input points.
	vector<POINT> pointsB(n);
	for (int i = 0; i < n; i++) {
		int x, y;
		scanf("%d%d", &x, &y);
		pointsB[i] = POINT(x, y);
	}

	//computing exec time (could find better way)
	auto convex_hull_pointsA = convex_hull(pointsA);
	auto convex_hull_pointsB = convex_hull(pointsB);

	printf("\n%lu\n", convex_hull_pointsA.size());
	for (POINT* point : convex_hull_pointsA) {
		point->print();
	}

	printf("\n%lu\n", convex_hull_pointsB.size());
	for (POINT* point : convex_hull_pointsB) {
		point->print();
	}

	auto convex_hull_pointsAB = mergeHulls(convex_hull_pointsA, convex_hull_pointsB);
	printf("\n%lu\n", convex_hull_pointsAB.size());
	for (POINT* point : convex_hull_pointsAB) {
		point->print();
	}

	return 0;
}
