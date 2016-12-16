#ifndef MERGE_HULL
#define MERGE_HULL

#include <vector>
#include "geometric_helpers.hh"
#include <stdio.h>
#include <algorithm>
#include "data_structures/convex_hull_representation.hh"


using namespace std;

int turn_right(POINT *a, POINT *b, POINT *c) {
    return DetPointer(a, b, c) < 0;
}

int turn_left(POINT *a, POINT *b, POINT *c) {
    return DetPointer(a, b, c) > 0;
}

int left_lower_tangent(ConvexHullRepresentation &hull, int index, POINT *b) {
    POINT *a = hull.get_point(index);
    POINT *u = hull.get_point(hull.go_counter_clockwise(index));
    POINT *w = hull.get_point(hull.go_clockwise(index));
    return turn_right(b, a, w) && !turn_left(b, a, u);
}

// A is always the left hull, B is the right one
int left_upper_tangent(ConvexHullRepresentation &left_hull, int aIndex, POINT *b) {
    POINT *a = left_hull.get_point(aIndex);
    POINT *u = left_hull.get_point(left_hull.go_counter_clockwise(aIndex));
    POINT *w = left_hull.get_point(left_hull.go_clockwise(aIndex));
    return turn_left(b, a, u) && !turn_right(b, a, w);
}

int lowerTangentB(ConvexHullRepresentation &right_hull, int index, POINT *a) {
    POINT *u = right_hull.get_point(right_hull.go_counter_clockwise(index));
    POINT *w = right_hull.get_point(right_hull.go_clockwise(index));
    POINT *b = right_hull.get_point(index);
    return !turn_right(a, b, w) && turn_left(a, b, u);
    //return left_upper_tangent(right_hull, left_hull, index, aIndex);
}

int right_upper_tangent(ConvexHullRepresentation &right_hull, int index, POINT *a) {
    POINT *b = right_hull.get_point(index);
    POINT *u = right_hull.get_point(right_hull.go_counter_clockwise(index));
    POINT *w = right_hull.get_point(right_hull.go_clockwise(index));
    return !turn_left(a, b, u) && turn_right(a, b, w);
}

int findLowerTangentA(ConvexHullRepresentation &left_hull, ConvexHullRepresentation &right_hull, int aIndex, int bIndex) {
    while (!left_lower_tangent(left_hull, aIndex, right_hull.get_point(bIndex))) {
        aIndex = left_hull.go_clockwise(aIndex);
    }
    return aIndex;
}

int findLowerTangentB(ConvexHullRepresentation &left_hull, ConvexHullRepresentation &right_hull, int aIndex, int bIndex) {
    while (!lowerTangentB(right_hull, bIndex, left_hull.get_point(aIndex))) {
        bIndex = right_hull.go_counter_clockwise(bIndex);
    }
    return bIndex;
}

int find_left_upper_tangent(ConvexHullRepresentation &hull, int index, POINT *p) {
    while (!left_upper_tangent(hull, index, p)) {
        index = hull.go_counter_clockwise(index);
    }
    return index;
}

int find_right_upper_tangent(ConvexHullRepresentation &hull, int index, POINT *p) {
    while (!right_upper_tangent(hull, index, p)) {
        index = hull.go_clockwise(index);
    }
    return index;
}

//TODO: make it more general
pair<int, int> findLowerT(ConvexHullRepresentation &left_hull, ConvexHullRepresentation &right_hull) {
    pair<int, int> returnValues;
    int left_index = left_hull.find_rightmost_point();
    int right_index = right_hull.find_leftmost_point();
    while (!(left_lower_tangent(left_hull, left_index, right_hull.get_point(right_index)) &&
             lowerTangentB(right_hull, right_index, left_hull.get_point(left_index)))) {
        left_index = find_right_upper_tangent(left_hull, left_index, right_hull.get_point(right_index));
        right_index = find_left_upper_tangent(right_hull, right_index, left_hull.get_point(left_index));
    }
    returnValues.first = left_index;
    returnValues.second = right_index;
    return returnValues;
}

pair<int, int> findUpperT(ConvexHullRepresentation &left_hull, ConvexHullRepresentation &right_hull) {
    pair<int, int> returnValues;
    int left_index = left_hull.find_rightmost_point();
    int right_index = right_hull.find_leftmost_point();
    while (!(left_upper_tangent(left_hull, left_index, right_hull.get_point(right_index)) &&
            right_upper_tangent(right_hull, right_index, left_hull.get_point(left_index)))) {
        left_index = find_left_upper_tangent(left_hull, left_index, right_hull.get_point(right_index));
        right_index = find_right_upper_tangent(right_hull, right_index, left_hull.get_point(left_index));
    }
    returnValues.first = left_index;
    returnValues.second = right_index;
    return returnValues;
}

#endif // MERGE_HULL
