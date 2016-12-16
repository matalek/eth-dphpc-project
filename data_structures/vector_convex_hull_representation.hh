#ifndef VECTOR_CONVEX_HULL_REPRESENTATION
#define VECTOR_CONVEX_HULL_REPRESENTATION

#include <vector>
#include <cassert>
#include <memory>

#include "../geometric_helpers.hh"
#include "../merge_hull.hh"
#include "convex_hull_representation.hh"
#include "representation_iterator.hh"

using namespace std;

class VectorConvexHullRepresentation : public ConvexHullRepresentation {

public:

    VectorConvexHullRepresentation(shared_ptr<vector<POINT*>> hull, bool upper)
            :  ConvexHullRepresentation(upper),hull(hull){};

	void merge(shared_ptr<ConvexHullRepresentation> hull) {
		shared_ptr<VectorConvexHullRepresentation> other_hull = std::static_pointer_cast<VectorConvexHullRepresentation>(hull);
		assert(other_hull != nullptr && "Merging two different subclasses");

        assert(other_hull->is_upper() == is_upper() && "Merging two different vectors");

		if(other_hull->is_upper()) {
            merge_upper_hull((*other_hull));
        }
		else {
            merge_lower_hull((*other_hull));
        }
	}

	/*methods for commont tangent alg*/

	int find_rightmost_point() override {
        return upper == true ? 0 : hull->size() -1;
	}

	int find_leftmost_point() override {
        return upper == true ? hull->size()-1 : 0;
	}

	int go_counter_clockwise(int index) override {
		return (index + 1) % gen_array_len();
	}

	int go_clockwise(int index) override {
		return index > 0 ? index - 1 : gen_array_len() -1;
	}

    int gen_array_len() {
        return hull->size();
    }

	POINT* get_point(int index) override {
        return hull->at(index);
    }

    shared_ptr<RepresentationIterator> get_iterator(int index) {
        // TODO: implement this method.
        return nullptr;
    }

	void merge_lower_hull(VectorConvexHullRepresentation &other_hull) {
		pair <int, int> low_tangent = findLowerT((*this), other_hull);
        vector<POINT*> mergedVector;
        mergedVector.reserve(low_tangent.first + 1 + other_hull.get_hull()->size()-low_tangent.second); // preallocate memory
        mergedVector.insert( mergedVector.end(), hull->begin(), hull->begin()+low_tangent.first+1);
        mergedVector.insert( mergedVector.end(), other_hull.get_hull()->begin() + low_tangent.second, other_hull.get_hull()->end());
        hull = shared_ptr<vector<POINT*> >(new vector<POINT*>(mergedVector));
	}

	void merge_upper_hull(VectorConvexHullRepresentation &other_hull) {
        pair <int, int> upper_tangent = findUpperT((*this), other_hull);
        vector<POINT*> mergedVector;
        mergedVector.reserve(upper_tangent.first + 1 + upper_tangent.second + 1); // preallocate memory
        mergedVector.insert( mergedVector.end(), other_hull.get_hull()->begin(), other_hull.get_hull()->begin() + upper_tangent.second + 1);
        mergedVector.insert( mergedVector.end(), hull->begin() + upper_tangent.first, hull.get()->end());
        hull = shared_ptr<vector<POINT*> >(new vector<POINT*>(mergedVector));
	}

    shared_ptr<vector<POINT*>> get_hull() override {
        return hull;
    }


protected:
	const int max_parallelism = 1;

private:
	shared_ptr<vector<POINT*>> hull;
};

#endif // VECTOR_CONVEX_HULL_REPRESENTATION