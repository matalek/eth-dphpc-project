#ifndef HULL_TREE_CONVEX_HULL_REPRESENTATION
#define HULL_TREE_CONVEX_HULL_REPRESENTATION

#include <vector>
#include <cassert>
#include <memory>

#include "../geometric_helpers.hh"
#include "../merge_hull.hh"
#include "convex_hull_representation.hh"

using namespace std;

class HullTreeConvexHullRepresentation : public ConvexHullRepresentation {

public:
    HullTreeConvexHullRepresentation() { };

    // Constructing object from convex hull points.
    HullTreeConvexHullRepresentation(shared_ptr<POINTS> points) {
        tree = shared_ptr<HullTreeNode>(new HullTreeNode(points, 0, points->size() - 1));
        tree->init();
    };

    shared_ptr<ConvexHullRepresentation> merge(shared_ptr<ConvexHullRepresentation> hull) override {
        return hull;
    }

    shared_ptr<vector<POINT*> > get_points() override {
        auto res = shared_ptr<vector<POINT*> >(new POINTS());
        if (!empty()) {
            auto node = tree->get_most_left();
            while (node) {
                res->push_back(node->get_point());
                node = node->get_succ();
            }
        }
        return res;
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

protected:
    const int max_parallelism = 1;

private:

public:
    pair<shared_ptr<HullTreeConvexHullRepresentation>, shared_ptr<HullTreeConvexHullRepresentation> > split(LL x0) {
        auto split_nodes = tree->split(x0);
        pair<shared_ptr<HullTreeConvexHullRepresentation>, shared_ptr<HullTreeConvexHullRepresentation> > res;
        return make_pair(shared_ptr<HullTreeConvexHullRepresentation>(new HullTreeConvexHullRepresentation(split_nodes.first)),
                shared_ptr<HullTreeConvexHullRepresentation>(new HullTreeConvexHullRepresentation(split_nodes.second)));
    }

private:

    shared_ptr<vector<POINT*> > points;

    // --------------------------------------------------
    // ----- Auxiliary class for node representation ----
    class HullTreeNode : public std::enable_shared_from_this<HullTreeNode> {
    public:
        HullTreeNode(shared_ptr<vector<POINT*> > points, int start, int end) {
            int n = end - start + 1;

            if (n == 1) {
                // Leaf in the tree.
                point = points->at(start);
                d = 1;
            } else {
                int l = 1;
                // Finding approximately half of the points number.
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
                // print();         
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
            }

            return make_pair(me, copy);
        }

        void print() {
            if ((!left) && (!right)) {
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

    private:
        // Updates values for node based on its children.
        void update_values() {
            if (!left || !right) {
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
        POINT* point; // Not null only for leafs;
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
    HullTreeConvexHullRepresentation(shared_ptr<HullTreeNode> tree) : tree(tree) { }
};


#endif // HULL_TREE_CONVEX_HULL_REPRESENTATION