#include <vector>
#include "geometric_helpers.hh"

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
	vector<POINT> points(n);
	for (int i = 0; i < n; i++) {
		int x, y;
		scanf("%d%d", &x, &y);
		points[i] = POINT(x, y);
	}

	//computing exec time (could find better way)
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	auto convex_hull_points = convex_hull(points);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	printf("TIME: ");
    	printf("%lld", duration_cast<microseconds>( t2 - t1 ).count());

	printf("\n%lu\n", convex_hull_points.size());
	for (POINT* point : convex_hull_points) {
		point->print();
	}

	return 0;
}
