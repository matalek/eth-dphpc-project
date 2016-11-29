#ifndef VECTOR_CONVEX_HULL_REPRESENTATION
#define VECTOR_CONVEX_HULL_REPRESENTATION

#include <vector>
#include <cassert>
#include <memory>

#include "../geometric_helpers.hh"
#include "../merge_hull.hh"
#include "convex_hull_representation.hh"

using namespace std;

class VectorConvexHullRepresentation : public ConvexHullRepresentation {

public:
	VectorConvexHullRepresentation() { };

	VectorConvexHullRepresentation(shared_ptr<vector<POINT*> > lower_hull, shared_ptr<vector<POINT*> > upper_hull)
			: lower_hull(lower_hull),upper_hull(upper_hull) {};

	shared_ptr<ConvexHullRepresentation> merge(shared_ptr<ConvexHullRepresentation> hull) override {
		shared_ptr<VectorConvexHullRepresentation> hull_vector = std::static_pointer_cast<VectorConvexHullRepresentation>(hull);
		assert(hull_vector != nullptr && "Merging two different subclasses");

		//vector<POINT*> res_points = mergeHulls((*this), (*hull_vector));
		merge_lower_hull((*hull_vector));
		merge_upper_hull((*hull_vector));
        return  shared_ptr<VectorConvexHullRepresentation>(new VectorConvexHullRepresentation(lower_hull,upper_hull));
		//return shared_ptr<VectorConvexHullRepresentation>(
		//		new VectorConvexHullRepresentation(shared_ptr<vector<POINT*> >(new vector<POINT*>(res_points))));
	}

	shared_ptr<vector<POINT*> > get_points() override {
        vector<POINT*> AB;
        int start = upper_hull.get()->at(0) == lower_hull.get()->at(lower_hull.get()->size()-1) ? 1 : 0;
        int end = lower_hull.get()->at(0) == upper_hull.get()->at(upper_hull.get()->size()-1)  ? 1 : 0;
        AB.reserve( lower_hull.get()->size() + upper_hull.get()->size()-start-end); // preallocate memory
        AB.insert( AB.end(), lower_hull.get()->begin(), lower_hull.get()->end());
        AB.insert( AB.end(), upper_hull.get()->begin() + start, upper_hull.get()->end() - end);
		return shared_ptr<vector<POINT*> >(new vector<POINT*>(AB));
	}

    shared_ptr<vector<POINT*> > get_lower_hull() {
        return lower_hull;
    }

    shared_ptr<vector<POINT*> > get_upper_hull() {
        return upper_hull;
    }

	/*methods for commont tangent alg*/

	int find_rightmost_point(bool upper) override {
		/*int index = 0;
		for (unsigned int i = 0; i < points.get()->size(); i++) {
			if (OrderXY(points.get()->at(index), points.get()->at(i))) {
				index = i;
			}
		}
		return index;
		*/
        return upper == true ? 0 : lower_hull.get()->size() -1;
	}

	int find_leftmost_point(bool upper) override {
		/*int index = 0;
		for (unsigned int i = 0; i < points.get()->size(); i++) {
			if (OrderXY(points.get()->at(i), points.get()->at(index))) {
				index = i;
			}
		}
		return index;*/
        return upper == true ? upper_hull.get()->size()-1 : 0;
	}

	int go_counter_clockwise(int index, bool upper) override {
		return (index + 1) % gen_array_len(upper);
	}

	int go_clockwise(int index, bool upper) override {
		return index > 0 ? index - 1 : gen_array_len(upper) -1;
	}

    int gen_array_len(bool upper) {
        return upper == true ? upper_hull.get()->size() : lower_hull.get()->size();
    }

	POINT* get_point(int index, bool upper) override {
        return upper == true ? upper_hull.get()->at(index) : lower_hull.get()->at(index);
    }

	void merge_lower_hull(VectorConvexHullRepresentation &other_hull) {
		pair <int, int> low_tangent = findLowerT((*this), other_hull);
        vector<POINT*> mergedVector;
        mergedVector.reserve(low_tangent.first + 1 + other_hull.get_lower_hull().get()->size()-low_tangent.second); // preallocate memory
        mergedVector.insert( mergedVector.end(), lower_hull.get()->begin(), lower_hull.get()->begin()+low_tangent.first+1);
        mergedVector.insert( mergedVector.end(), other_hull.get_lower_hull().get()->begin() + low_tangent.second, other_hull.get_lower_hull().get()->end());
        /*int hullIndex = 0;
        while (1) {
            mergedVector.push_back(get_point(hullIndex, false));
            if(hullIndex == low_tangent.first) break;
            hullIndex = go_counter_clockwise(hullIndex, false);
        };
        hullIndex = low_tangent.second;
        while (1) {
            mergedVector.push_back(other_hull.get_point(hullIndex, false));
            hullIndex = other_hull.go_counter_clockwise(hullIndex, false);
            if(hullIndex == 0) break;
        };*/
        lower_hull = shared_ptr<vector<POINT*> >(new vector<POINT*>(mergedVector));
	}

	void merge_upper_hull(VectorConvexHullRepresentation &other_hull) {
        pair <int, int> upper_tangent = findUpperT((*this), other_hull);
        vector<POINT*> mergedVector;
        /*int hull_index = 0;
        while (1) {
            mergedVector.push_back(other_hull.get_point(hull_index, true));
            if(hull_index == upper_tangent.second) break;
            hull_index = other_hull.go_counter_clockwise(hull_index, true);
        };
        hull_index = upper_tangent.first;
        while (1) {
            mergedVector.push_back(get_point(hull_index, true));
            hull_index = go_counter_clockwise(hull_index, true);
            if(hull_index == 0) break;
        };
        */
        mergedVector.reserve(upper_tangent.first + 1 + upper_tangent.second + 1); // preallocate memory
        mergedVector.insert( mergedVector.end(), other_hull.get_upper_hull().get()->begin(), other_hull.get_upper_hull().get()->begin() + upper_tangent.second + 1);
        mergedVector.insert( mergedVector.end(), upper_hull.get()->begin() + upper_tangent.first, upper_hull.get()->end());
        upper_hull = shared_ptr<vector<POINT*> >(new vector<POINT*>(mergedVector));
	}



protected:
	const int max_parallelism = 1;

private:
	shared_ptr<vector<POINT*> > lower_hull;
    shared_ptr<vector<POINT*> > upper_hull;
	bool upper;

};

#endif // VECTOR_CONVEX_HULL_REPRESENTATION