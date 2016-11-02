#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/ch_graham_andrew.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
using namespace std;
int main() {
	int n;
	cin >> n;

	// Reading from standard input points.
	vector<Point_2> points(n);
	for (int i = 0; i < n; i++) {
		int x, y;
		cin >> x >> y;
		points[i] = Point_2(x, y);
	}

	vector<Point_2> result;
	CGAL::ch_graham_andrew( points.begin(), points.end(), std::back_inserter(result));
	printf("\n%lu\n", result.size());

	for (Point_2 point : result) {
        cout << (long long int) point.x() << " " << (long long int) point.y() << "\n";
	}
	return 0;
}
