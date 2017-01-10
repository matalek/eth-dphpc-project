#ifndef HULL_TREE_CONVEX_HULL_REPRESENTATION
#define HULL_TREE_CONVEX_HULL_REPRESENTATION

#include <vector>
#include <cassert>
#include <memory>
#include <omp.h>

#include "../geometric_helpers.hh"
#include "../parallel_helper.hh"
#include "../merge_hull.hh"
#include "convex_hull_representation.hh"
#include "representation_iterator.hh"

using namespace std;

class HullTreeConvexHullRepresentation : public ConvexHullRepresentation {

public:
    HullTreeConvexHullRepresentation(bool upper) : ConvexHullRepresentation(upper) { };

    // Constructing object from convex hull points.
    HullTreeConvexHullRepresentation(shared_ptr<POINTS> points, bool upper)
            : ConvexHullRepresentation(upper) {

        if (upper) {
            // TODO(matalek): maybe think about something more efficient.
            reverse(points->begin(), points->end());
        }

        // Allocate memory in advance (mapping to virtual memory).
        HullTreeNode* temp = new HullTreeNode[4 * points->size()];
        int a = (1 + 2 + 3 -6)*7;
        if (a) cout << temp[0].get_point()->x;
        delete [] temp;

        tree = shared_ptr<HullTreeNode>(new HullTreeNode(points, 0, points->size() - 1));
        tree->init();
    };

    void merge(shared_ptr<ConvexHullRepresentation> hull) { }

    shared_ptr<vector<POINT*> > get_hull() override {
        auto res = shared_ptr<vector<POINT*> >(new POINTS());
        if (!empty()) {
            auto node = tree->get_most_left();
            while (node) {
                res->push_back(node->get_point());
                node = node->get_succ();
            }
            if (upper) {
                reverse(res->begin(), res->end());
            }
        }
        return res;
    }

    /*methods for commont tangent alg*/

    int find_rightmost_point() override {
        return upper ? 0 : size() - 1;
    }

    int find_leftmost_point() override {
        return upper ? size() - 1 : 0;
    }

    int go_counter_clockwise(int index) override {
        if (index < size() - 1) {
            return index + 1;
        }
        return 0;
    }

    int go_clockwise(int index) override {
        if (index > 0) {
            return index - 1;
        }
        return size() - 1;
    }

    POINT* get_point(int index) override {
        if (upper) {
            index = size() - 1 - index;
        }
        return tree->find_node(index)->get_point();
    }

    shared_ptr<RepresentationIterator> get_iterator(int index) {
        return shared_ptr<RepresentationIterator>(new HullTreeIterator(tree, index, size(), upper));
    }

    void print() const {
        if (empty()) {
            printf("empty\n");
        } else {
            tree->print();
        }
    }

    bool empty() const {
        return !tree;
    }

    int size() const {
        return tree->get_d();
    }

    shared_ptr<HullTreeConvexHullRepresentation> trim(int left, int right) {
        POINT* left_p = get_point(left), * right_p = get_point(right);

        auto first_split = split(right_p->x);

        // TODO(matalek): very ugly, think about something different
        auto second_split = first_split.first->split(left_p->x - 1);

        return second_split.second;
    }

    pair<shared_ptr<HullTreeConvexHullRepresentation>, shared_ptr<HullTreeConvexHullRepresentation> > split(LL x0) {
        auto split_nodes = tree->split(x0);
        pair<shared_ptr<HullTreeConvexHullRepresentation>, shared_ptr<HullTreeConvexHullRepresentation> > res;
        return make_pair(shared_ptr<HullTreeConvexHullRepresentation>(new HullTreeConvexHullRepresentation(split_nodes.first, upper)),
                shared_ptr<HullTreeConvexHullRepresentation>(new HullTreeConvexHullRepresentation(split_nodes.second, upper)));
    }

    // Merges hull trees, some of which might be empty. This method does not remove
    // any points - it only creates a simple union of points.
    static shared_ptr<HullTreeConvexHullRepresentation> merge_hulls(shared_ptr<HullTreeConvexHullRepresentation>* hulls, int n) {
        int* sum = new int[n];
        #pragma omp parallel num_threads(n)
        {
            int id = omp_get_thread_num();
            sum[id] = !(hulls[id]->empty());
        }

        int* sum_pref = ParallelHelper::prefix_sum(sum, n);
        int non_empty_cnt = sum_pref[n - 1];

        shared_ptr<HullTreeConvexHullRepresentation>* non_empty =
                new shared_ptr<HullTreeConvexHullRepresentation>[non_empty_cnt];

        #pragma omp parallel num_threads(n)
        {
            int id = omp_get_thread_num();
            if (!hulls[id]->empty()) {
                non_empty[sum_pref[id] - 1] = hulls[id];
            }
        }

        if (!non_empty_cnt) {
            return empty_hull(hulls[0]->is_upper());
        }
        shared_ptr<HullTreeConvexHullRepresentation> res = merge_non_empty_hulls(non_empty, non_empty_cnt);

        delete [] sum;
        delete [] sum_pref;
        delete [] non_empty;

        return res;
    }

    // Merges hulls, which are non-empty. As above - it only creates union, without
    // executing tangent lines algorithm.
    static shared_ptr<HullTreeConvexHullRepresentation> merge_non_empty_hulls(shared_ptr<HullTreeConvexHullRepresentation>* hulls, int n) {
        omp_set_nested(1);
        return shared_ptr<HullTreeConvexHullRepresentation>(
                new HullTreeConvexHullRepresentation(HullTreeNode::merge_non_empty_hulls(hulls, 0, n - 1), hulls[0]->is_upper()));
    }

protected:
    const int max_parallelism = 1;

private:

    shared_ptr<vector<POINT*> > points;

    // --------------------------------------------------
    // ----- Auxiliary class for node representation ----
    class HullTreeNode : public std::enable_shared_from_this<HullTreeNode> {
    public:
        HullTreeNode() { }

        HullTreeNode(shared_ptr<vector<POINT*> > points, int start, int end) {
            int n = end - start + 1;

            if (n == 1) {
                // Leaf in the tree.
                point = points->at(start);
                d = 1;
            } else {
                // Finding approximately half of the points number.
                int l = 1;
                while (l << 1 < n) {
                    l <<= 1;
                }
                left = shared_ptr<HullTreeNode>(new HullTreeNode(points, start, start + l - 1));
                left->init();
                right = shared_ptr<HullTreeNode>(new HullTreeNode(points, start + l, end));
                right->init();

                update_values();
                left->most_right->succ = right->most_left;
                right->most_left->prev = left->most_right;
            }
        }

        // Call this method after constructing an object. It is necessary only when
        // HullTreeNode corresponds to one point.
        // We cannot do it in constructor, since shared_from_this() method
        // is not accessible from constructor.
        void init() {
            if (!left && !right) {
                most_left = shared_from_this();
                most_right = shared_from_this();
            }
        }

        HullTreeNode(const HullTreeNode& tree) : d(tree.d), point(tree.point), left(tree.left), right(tree.right),
                most_left(tree.most_left), most_right(tree.most_right), prev(tree.prev), succ(tree.succ) { }

        static shared_ptr<HullTreeNode> merge_non_empty_hulls(shared_ptr<HullTreeConvexHullRepresentation>* hulls,
                int start, int end) {
            int n = end - start + 1;

            if (n == 1) {
                return hulls[start]->tree;
            }

            // Finding approximately half of the hulls number.
            int l = 1;
            while (l << 1 < n) {
                l <<= 1;
            }

            shared_ptr<HullTreeNode> res = shared_ptr<HullTreeNode>(new HullTreeNode());
            #pragma omp parallel num_threads(2)
            {
                int id = omp_get_thread_num();
                if (id) {
                    res->left = merge_non_empty_hulls(hulls, start, start + l - 1);
                } else {
                    res->right = merge_non_empty_hulls(hulls, start + l, end);
                }
            }

            res->update_values();
            res->left->most_right->succ = res->right->most_left;
            res->right->most_left->prev = res->left->most_right;
            return res;
        }

        // We call this method resursively, going alond the path looking for x0.
        // Returns original and copy of the node on the path.
        pair<shared_ptr<HullTreeNode>, shared_ptr<HullTreeNode> > split(LL x0) {
            shared_ptr<HullTreeNode> copy = shared_ptr<HullTreeNode>(new HullTreeNode(*this));
            shared_ptr<HullTreeNode> me = shared_from_this();
            if ((!left) && (!right)) {
                // Leaf.
                if (point->x > x0) {
                    // Should only happen when all the points in the tree have x-coordintae greater
                    // than x0. Then we are first point.
                    me = nullptr;
                } else {
                    if (succ != nullptr) {
                        succ->prev = nullptr;
                    }
                    succ = nullptr;
                    copy = nullptr;
                }
                return make_pair(me, copy);
            } else if (!left) {
                // Going only right.
                auto rec = right->split(x0);

                right = rec.first;

                copy->right = rec.second;
            } else if (!right) {
                // Going only left.
                auto rec = left->split(x0);

                left = rec.first;

                copy->left = rec.second;
            } else {
                pair<shared_ptr<HullTreeNode>, shared_ptr<HullTreeNode> > rec;
                if (right->most_left->point->x > x0) {
                    // Going left.
                    rec = left->split(x0);

                    left = rec.first;
                    right = nullptr;

                    copy->left = rec.second;
                } else {
                    // Going right.
                    rec = right->split(x0);

                    right = rec.first;

                    copy->right = rec.second;
                    copy->left = nullptr;
                }
            }

            if (empty()) {
                me = nullptr;
            } else {
                update_values();
            }

            if (copy->empty()) {
                copy = nullptr;
            } else {
                copy->update_values();
            }

            return make_pair(me, copy);
        }

        shared_ptr<HullTreeNode> find_node(int index) {
            if (!left && !right) {
                return shared_from_this();
            }
            int left_d = 0;
            if (left) {
                left_d = left->d;
            }
            if (left_d > index) {
                // Going left.
                return left->find_node(index);
            }
            // Going right
            return right->find_node(index - left_d);
        }

        void print() {
            if ((!left) && (!right)) {
                // TODO(matalek): delete when measuring performance.
                assert(!!point);
                point->print();
            } else {
                if (left) {
                    left->print();
                }
                if (right) {
                    right->print();
                }
            }
        }

        POINT* get_point() const{
            return point;
        }

        shared_ptr<HullTreeNode> get_succ() const{
            return succ;
        }

        shared_ptr<HullTreeNode> get_prev() const{
            return prev;
        }

        shared_ptr<HullTreeNode> get_most_left() const{
            return most_left;
        }

        shared_ptr<HullTreeNode> get_most_right() const{
            return most_right;
        }

        int get_d() const {
            return d;
        }

    private:
        // Updates values for node based on its children.
        void update_values() {
            if (!left && !right) {
                return;
            }
            most_left = left ? left->most_left : right->most_left;
            most_right = right ? right->most_right : left->most_right;
            d = (left ? left->d : 0) + (right ? right->d : 0);
        }

        bool empty() {
            return !point && !left && !right;
        }

        int d;
        POINT* point = nullptr; // Not null only for leafs;
        shared_ptr<HullTreeNode> left, right;
        // most_left corresponds to m from the data structure description. We keep
        // also most_right to enable calculating prev and succ efficiently.
        shared_ptr<HullTreeNode> most_left, most_right;
        // Not null only for leafs.
        shared_ptr<HullTreeNode> prev, succ;
    };
    // --------------------------------------------------

    shared_ptr<HullTreeNode> tree;

    // Constructing object based on the tree node.
    HullTreeConvexHullRepresentation(shared_ptr<HullTreeNode> tree, bool upper)
            : ConvexHullRepresentation(upper), tree(tree) { }

    shared_ptr<HullTreeNode> find_node(int index) {
        return tree->find_node(index);
    }

    class HullTreeIterator : public RepresentationIterator {

    public:
        HullTreeIterator(shared_ptr<HullTreeNode> root, int index, int n, bool upper)
                : root(root), index(index), n(n), upper(upper) {
            if (upper) {
                index = n - 1 - index;
            }
            node = root->find_node(index);
        }

        POINT* get_point() {
            return node->get_point();
        }

        void go_clockwise() {
            node = get_clockwise();
            if (index > 0) {
                index--;
            } else {
                index = n - 1;
            }
        }

        void go_counter_clockwise() {
            node = get_counter_clockwise();
            if (index < n - 1) {
                index++;
            } else {
                index = 0;
            }
        }

        POINT* get_point_clockwise() {
            return get_clockwise()->get_point();
        }

        POINT* get_point_counter_clockwise() {
            return get_counter_clockwise()->get_point();
        }

        int get_index() {
            return index;
        }

    private:
        shared_ptr<HullTreeNode> root, node;
        int index, n;
        bool upper;

        shared_ptr<HullTreeNode> get_clockwise() {
            return (upper) ? get_succ() : get_prev();
        }

        shared_ptr<HullTreeNode> get_counter_clockwise() {
            return (upper) ? get_prev() : get_succ();
        }

        shared_ptr<HullTreeNode> get_succ() {
            auto candidate = node->get_succ();
            if (!candidate) {
                return root->get_most_left();
            }
            return candidate;
        }

        shared_ptr<HullTreeNode> get_prev() {
            auto candidate = node->get_prev();
            if (!candidate) {
                return root->get_most_right();
            }
            return candidate;
        }
    };

public:
    static shared_ptr<HullTreeConvexHullRepresentation> empty_hull(bool upper) {
        return shared_ptr<HullTreeConvexHullRepresentation>(
                new HullTreeConvexHullRepresentation((shared_ptr<HullTreeNode>) nullptr, upper));
    }


};


#endif // HULL_TREE_CONVEX_HULL_REPRESENTATION