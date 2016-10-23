// Common geometric helpers to use in different algorithms.
// Auxiliary type definitions.
#ifndef GEOMETRIC_HELPERS_HH
#define GEOMETRIC_HELPERS_HH

#include <stdio.h>

typedef long long int LL;

// Macro for calculating cross product.
#define Det(a, b, c) (LL(b.x - a.x) * LL(c.y - a.y) \
		- LL(b.y - a.y) * LL(c.x - a.x))

#define abs(x) (x > 0 ? x : -x)

// Struct representing point with integer coordinates.
struct POINT {
	int x, y;

	POINT(int x = 0, int y = 0) : x(x), y(y) { }
	
	bool operator ==(const POINT& a) {
		return a.x == x && a.y == y; 
	}

	void print() {
		printf("%d %d\n", x, y);
	}
};

// Function designating order with regards to coordinates of
// pointers to points set.
bool OrderXY(const POINT* a, const POINT* b) {
	return a->x == b->x ? a->y < b->y : a->x < b->x;
}

#endif // GEOMETRIC_HELPERS_HH
