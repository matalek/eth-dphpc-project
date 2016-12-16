#ifndef MERGE_HULL
#define MERGE_HULL

#include <vector>
#include "geometric_helpers.hh"
#include <stdio.h>
#include <algorithm>
#include "data_structures/convex_hull_representation.hh"
#include "data_structures/representation_iterator.hh"


using namespace std;

int turn_right(POINT *a, POINT *b, POINT *c) {
    return DetPointer(a, b, c) < 0;
}

int turn_left(POINT *a, POINT *b, POINT *c) {
    return DetPointer(a, b, c) > 0;
}

int left_lower_tangent(shared_ptr<RepresentationIterator> iterator, POINT *b) {
    POINT *a = iterator->get_point();
    POINT *u = iterator->get_point_counter_clockwise();
    POINT *w = iterator->get_point_clockwise();
    return turn_right(b, a, w) && !turn_left(b, a, u);
}

// A is always the left hull, B is the right one
int left_upper_tangent(shared_ptr<RepresentationIterator> iterator, POINT *b) {
    POINT *a = iterator->get_point();
    POINT *u = iterator->get_point_counter_clockwise();
    POINT *w = iterator->get_point_clockwise();
    return turn_left(b, a, u) && !turn_right(b, a, w);
}

int lowerTangentB(shared_ptr<RepresentationIterator> iterator, POINT *a) {
    POINT *u = iterator->get_point_counter_clockwise();
    POINT *w = iterator->get_point_clockwise();
    POINT *b = iterator->get_point();
    return !turn_right(a, b, w) && turn_left(a, b, u);
}

int right_upper_tangent(shared_ptr<RepresentationIterator> iterator, POINT *a) {
    POINT *b = iterator->get_point();
    POINT *u = iterator->get_point_counter_clockwise();
    POINT *w = iterator->get_point_clockwise();
    return !turn_left(a, b, u) && turn_right(a, b, w);
}

/*int findLowerTangentA(ConvexHullRepresentation &left_hull, ConvexHullRepresentation &right_hull, int aIndex, int bIndex) {
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
}*/

void find_left_upper_tangent( shared_ptr<RepresentationIterator> iterator, POINT *p) {
    while (!left_upper_tangent(iterator, p)) {
        iterator->go_counter_clockwise();
    }
}


void find_right_upper_tangent(shared_ptr<RepresentationIterator> iterator, POINT *p) {
    while (!right_upper_tangent(iterator, p)) {
        iterator->go_clockwise();
    }
}

pair<int, int> findLowerT(ConvexHullRepresentation &left_hull, ConvexHullRepresentation &right_hull) {
    pair<int, int> returnValues;
    shared_ptr<RepresentationIterator> left_iter = left_hull.get_iterator(left_hull.find_rightmost_point());
    shared_ptr<RepresentationIterator> right_iter = right_hull.get_iterator(right_hull.find_leftmost_point());
    while (!(right_upper_tangent(left_iter, right_iter->get_point()) &&
             lowerTangentB(right_iter, left_iter->get_point()))) {
        find_right_upper_tangent(left_iter, right_iter->get_point());
        find_left_upper_tangent(right_iter, left_iter->get_point());
    }
    returnValues.first = left_iter->get_index();
    returnValues.second = right_iter->get_index();
    return returnValues;
}

pair<int, int> findUpperT(ConvexHullRepresentation &left_hull, ConvexHullRepresentation &right_hull) {
    pair<int, int> returnValues;
    shared_ptr<RepresentationIterator> left_iter = left_hull.get_iterator(left_hull.find_rightmost_point());
    shared_ptr<RepresentationIterator> right_iter = right_hull.get_iterator(right_hull.find_leftmost_point());
    while (!(left_upper_tangent(left_iter, right_iter->get_point()) &&
            right_upper_tangent(right_iter, left_iter->get_point()))) {
        find_left_upper_tangent(left_iter, right_iter->get_point());
        find_right_upper_tangent(right_iter, left_iter->get_point());
    }
    returnValues.first = left_iter->get_index();
    returnValues.second = right_iter->get_index();
    return returnValues;
}

#endif // MERGE_HULL
