#ifndef CONVEX_HULL_REPRESENTATION
#define CONVEX_HULL_REPRESENTATION

#include <vector>
#include <cassert>
#include <memory>

#include "../geometric_helpers.hh"

using namespace std;

// Represents abstract convex hull representation.
// Possible subclasses have to implement also constructor which takes vector of points.
// TODO(matalek): change that it will take shared_ptr to vector of points.
class ConvexHullRepresentation {

public:
	virtual ~ConvexHullRepresentation() { };

	virtual shared_ptr<ConvexHullRepresentation> merge(shared_ptr<ConvexHullRepresentation>) = 0;

	// TODO(matalek): change to return shared_ptr.
	virtual vector<POINT*> get_points() = 0;

	int get_max_parallelism() {
		return max_parallelism;
	}

	void set_parallelism(int parallelism) {
		assert(parallelism > 0 && "Parallelism must be greater than 0.");
		assert(parallelism <= max_parallelism && "Too high parallelism.");
		this->parallelism = parallelism;
	}

protected:
	int max_parallelism;
	int parallelism = 1;
};

#endif // CONVEX_HULL_REPRESENTATION