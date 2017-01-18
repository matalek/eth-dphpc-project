#ifndef GENERATOR
#define GENERATOR

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

using namespace std;

class Generator {
public:
	virtual ~Generator() { };

	virtual vector<POINT> generate_points(int n) = 0;

protected:
	static const int range = 1000000000;

	struct points_compare {
		bool operator() (const POINT a, const POINT b) {
			return a.x == b.x ? 0 : a.x < b.x;
		}
	};

	int rand_coordinate(int range) {
		return (rand() % (2 * range + 1)) - range;
	}

};

#endif // GENERATOR
