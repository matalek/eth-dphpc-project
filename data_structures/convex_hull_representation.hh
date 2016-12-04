#ifndef CONVEX_HULL_REPRESENTATION
#define CONVEX_HULL_REPRESENTATION

#include <vector>
#include <cassert>
#include <memory>

#include "../geometric_helpers.hh"

using namespace std;

// Represents abstract convex hull representation.
// Possible subclasses have to implement also constructor which takes 
// a shared pointer of vector of points.
class ConvexHullRepresentation {

public:
	virtual ~ConvexHullRepresentation() { };

	//virtual shared_ptr<ConvexHullRepresentation> merge(shared_ptr<ConvexHullRepresentation>) = 0;

	virtual shared_ptr<vector<POINT*> > get_hull() = 0;

	int get_max_parallelism() {
		return max_parallelism;
	}

	void set_parallelism(int parallelism) {
		assert(parallelism > 0 && "Parallelism must be greater than 0.");
		assert(parallelism <= max_parallelism && "Too high parallelism.");
		this->parallelism = parallelism;
	}

	/*methods for commont tangent alg*/

	virtual int find_rightmost_point() = 0;
	virtual int find_leftmost_point() = 0;
	virtual POINT* get_point(int index) = 0;
	virtual int go_counter_clockwise(int index) = 0;
	virtual int go_clockwise(int index) = 0;
	//virtual void merge_lower_hull(ConvexHullRepresentation &other_hull) = 0;
	//virtual void merge_upper_hull(ConvexHullRepresentation &other_hull) = 0;

protected:
	int max_parallelism;
	int parallelism = 1;
};

#endif // CONVEX_HULL_REPRESENTATION