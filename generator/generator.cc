// Simple points generator, taking number of points and range.
// Will generate points from [-range, range].

#include <vector>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <iostream>
#include "../geometric_helpers.hh"

#include "generator_loader.hh"

using namespace std;

int main(int argc, char* argv[]) {
	// Number of points.
	int n;
	cin >> n;

	string shape;
	if (argc < 2) {
		shape = "square";
	} else {
		shape = argv[1];
	}

	Generator* generator = load_generator(shape);
	vector<POINT> points = generator->generate_points(n);
	delete(generator);

	cout << n << "\n";
	for (vector<POINT>::iterator it = points.begin(); it != points.end(); ++it) {
		it->print();
	}

	return 0;
}
