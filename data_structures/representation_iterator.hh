#ifndef REPRESENTATION_ITERATOR
#define REPRESENTATION_ITERATOR

#include <vector>
#include <cassert>
#include <memory>

#include "../geometric_helpers.hh"

using namespace std;

// Represents abstract convex hull representation.
// Possible subclasses have to implement also constructor which takes
// a shared pointer of vector of points.
class RepresentationIterator {

public:

	virtual POINT* get_point() = 0;
	virtual void go_clockwise() = 0;
	virtual void go_counter_clockwise() = 0;
	virtual POINT* get_point_clockwise() = 0;
	virtual POINT* get_point_counter_clockwise() = 0;
};

#endif // REPRESENTATION_ITERATOR
