#ifndef HULL_TREE_CONVEX_HULL_REPRESENTATION
#define HULL_TREE_CONVEX_HULL_REPRESENTATION

#include <vector>
#include <cassert>
#include <memory>
#include <iostream>

#include "../geometric_helpers.hh"
#include "../merge_hull.hh"
#include "convex_hull_representation.hh"

using namespace std;

class HullTreeConvexHullRepresentation : public ConvexHullRepresentation {

public:
	HullTreeConvexHullRepresentation() { };

	HullTreeConvexHullRepresentation(shared_ptr<POINTS> points) {
		tree = shared_ptr<HullTreeNode>(new HullTreeNode(points, 0, points->size() - 1));
	};

	shared_ptr<ConvexHullRepresentation> merge(shared_ptr<ConvexHullRepresentation> hull) override {
		return hull;
	}

	shared_ptr<vector<POINT*> > get_points() override {
		return points;
	}

	/*methods for commont tangent alg*/

	int find_rightmost_point() override {
		int index = 0;
		return index;
	}

	int find_leftmost_point() override {
		int index = 0;
		return index;
	}

	int go_counter_clockwise(int index) override {
		return 0;
	}

	int go_clockwise(int index) override {
		return 0;
	}

	POINT* get_point(int index) override {
        return nullptr;
    }

protected:
	const int max_parallelism = 1;

private:
	shared_ptr<vector<POINT*> > points;

	class HullTreeNode {
	public:
		HullTreeNode(shared_ptr<vector<POINT*> > points, int start, int end) {
			int n = end - start + 1;

			if (n == 1) {
				// Leaf in the tree.
				most_left = shared_ptr<HullTreeNode>(this);
				most_right = shared_ptr<HullTreeNode>(this);
				point = points->at(start);
				d = 1;
			} else {
				int l = 1;
				// Finding approximately size of half of the points.
				while (l << 1 < n) {
					l <<= 1;
				}
				left = shared_ptr<HullTreeNode>(new HullTreeNode(points, start, start + l - 1));
				right = shared_ptr<HullTreeNode>(new HullTreeNode(points, start + l, end));
				d = left->d + right->d;
				most_left = left->most_left;
				most_right= right->most_right;
				left->most_right->succ = right->most_left;
				right->most_left->prev = left->most_right;
				cout << d << "\n";
			}
		}

	private:
		int d;
		POINT* point; // Not null only for leafs;
		shared_ptr<HullTreeNode> left, right;
		// most_left corresponds to m from the data structure description. We keep
		// also most_right to enable calculating prev and succ efficiently.
		shared_ptr<HullTreeNode> most_left, most_right; 
		shared_ptr<HullTreeNode> prev, succ;
	};

	shared_ptr<HullTreeNode> tree;

};


#endif // HULL_TREE_CONVEX_HULL_REPRESENTATION