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

shared_ptr<RepresentationIterator>
binary_right_upper_tangent(ConvexHullRepresentation &hull, shared_ptr<RepresentationIterator> right_iter, int to, POINT *P) {
    int a, b, c;
    int dnC;

    if (right_upper_tangent(right_iter, P))
        return right_iter;

    a = right_iter->get_index();
    b = to;

    while (true) {
        c = (a + b) / 2;
        shared_ptr<RepresentationIterator> iterator = hull.get_iterator(c);
        dnC = turn_right(P, iterator->get_point(), iterator->get_point_clockwise());
        if (dnC && !turn_left(P, hull.get_point(c), iterator->get_point_counter_clockwise())) {
            return iterator;
        } else {
            if (dnC) {
                b = c;
            } else {
                a = c;
            }
        }
    }
}

shared_ptr<RepresentationIterator>
binary_left_upper_tangent(ConvexHullRepresentation &hull, shared_ptr<RepresentationIterator> left_iter, int to, POINT *P) {
    int a, b, c;
    int dnC;

    if (left_upper_tangent(left_iter, P))
        return left_iter;

    a = left_iter->get_index();
    b = to;

    while (true) {
        c = (a + b) / 2;
        shared_ptr<RepresentationIterator> iterator = hull.get_iterator(c);
        dnC = turn_left(P, hull.get_point(c), iterator->get_point_counter_clockwise());
        if (dnC && !turn_right(P, iterator->get_point(), iterator->get_point_clockwise())) {
            return iterator;
        } else {
            if (dnC) {
                b = c;
            } else {
                a = c;
            }
        }
    }
}

void find_left_upper_tangent(shared_ptr<RepresentationIterator> iterator, POINT *p) {
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
    pair<int, int> return_values;
    shared_ptr<RepresentationIterator> left_iter = left_hull.get_iterator(left_hull.find_rightmost_point());
    shared_ptr<RepresentationIterator> right_iter = right_hull.get_iterator(right_hull.find_leftmost_point());

    while (!(right_upper_tangent(left_iter, right_iter->get_point()) &&
             lowerTangentB(right_iter, left_iter->get_point()))) {
        find_right_upper_tangent(left_iter, right_iter->get_point());
        find_left_upper_tangent(right_iter, left_iter->get_point());
    }

    return_values.first = left_iter->get_index();
    return_values.second = right_iter->get_index();
    return return_values;
}

pair<int, int> findUpperT(ConvexHullRepresentation &left_hull, ConvexHullRepresentation &right_hull) {
    pair<int, int> return_values;
    shared_ptr<RepresentationIterator> left_iter = left_hull.get_iterator(left_hull.find_rightmost_point());
    shared_ptr<RepresentationIterator> right_iter = right_hull.get_iterator(right_hull.find_leftmost_point());

    while (!(left_upper_tangent(left_iter, right_iter->get_point()) &&
             right_upper_tangent(right_iter, left_iter->get_point()))) {
        find_left_upper_tangent(left_iter, right_iter->get_point());
        find_right_upper_tangent(right_iter, left_iter->get_point());
    }

    return_values.first = left_iter->get_index();
    return_values.second = right_iter->get_index();
    return return_values;
}

#endif // MERGE_HULL
