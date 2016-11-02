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

//how to split N things into M parts:  lenght of i part = (N*i+N)/M - (N*i)/M
int getPartLenght(int n, int m, int i){
    return (n*i+n)/m - (n*i)/m;
}

void readPoints(vector<POINT>& points, int partLenght) {
    for (int i = 0; i < partLenght; i++) {
        int x, y;
        scanf("%d%d", &x, &y);
        points[i] = POINT(x, y);
    }
}




int main() {

	using namespace std;

	using namespace std::chrono;

    int m = 2;
	// Number of points.
    int n;
	scanf("%d", &n);
	// Reading from standard input points.
    int partIndex = 0;
    vector<POINT> pointsA(getPartLenght(n,m,partIndex));
    readPoints(pointsA, getPartLenght(n,m,partIndex++));
	vector<POINT> pointsB(getPartLenght(n,m,partIndex));
    readPoints(pointsB, getPartLenght(n,m,partIndex++));

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
	//computing exec time (could find better way)
	auto convex_hull_pointsA = convex_hull(pointsA);
	auto convex_hull_pointsB = convex_hull(pointsB);
    auto convex_hull_pointsAB = mergeHulls(convex_hull_pointsA, convex_hull_pointsB);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    printf("TIME: ");
	printf("%lld", duration_cast<microseconds>( t2 - t1 ).count());

	printf("\n%lu\n", convex_hull_pointsAB.size());
	for (POINT* point : convex_hull_pointsAB) {
		point->print();
	}

	return 0;
}
