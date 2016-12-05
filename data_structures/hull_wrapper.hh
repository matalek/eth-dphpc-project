#ifndef HULL_WRAPPER
#define HULL_WRAPPER

#include <vector>
#include <cassert>
#include <memory>

#include "../geometric_helpers.hh"
#include "convex_hull_representation.hh"
#include "vector_convex_hull_representation.hh"

using namespace std;


shared_ptr<vector<POINT*> > create_complet_hull(shared_ptr<vector<POINT*>> lower_hull, shared_ptr<vector<POINT*>> upper_hull) {
	vector<POINT*> AB;
	int start = upper_hull->at(0) == lower_hull->at(lower_hull->size()-1) ? 1 : 0;
	int end = lower_hull->at(0) == upper_hull->at(upper_hull->size()-1)  ? 1 : 0;
	AB.reserve( lower_hull->size() + upper_hull.get()->size()-start-end); // preallocate memory
	AB.insert( AB.end(), lower_hull->begin(), lower_hull->end());
	AB.insert( AB.end(), upper_hull->begin() + start, upper_hull->end() - end);
	return shared_ptr<vector<POINT*> >(new vector<POINT*>(AB));
}


//template <class R>
class HullWrapper{

public:
	HullWrapper(shared_ptr<VectorConvexHullRepresentation> upper_hull, shared_ptr<VectorConvexHullRepresentation> lower_hull) :
		upper_hull(upper_hull), lower_hull(lower_hull) {};
	// it might be done parallel
	void merge(shared_ptr<HullWrapper> other) {
		//check upper && lower are the same
		upper_hull->merge(other->get_upper_hull());
		lower_hull->merge(other->get_lower_hull());
	};
	// returns merged upper and lower hull and remove duplicate points at the boundaries
	shared_ptr<vector<POINT*>> get_points() {
		return create_complet_hull(lower_hull->get_hull(), upper_hull->get_hull());
	}

	shared_ptr<VectorConvexHullRepresentation> get_lower_hull() {
		return lower_hull;
	}

	shared_ptr<VectorConvexHullRepresentation> get_upper_hull() {
		return upper_hull;
	}
private:
	shared_ptr<VectorConvexHullRepresentation> upper_hull;
	shared_ptr<VectorConvexHullRepresentation> lower_hull;
};


#endif // HULL_WRAPPER