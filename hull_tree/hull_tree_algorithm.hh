#ifndef HULL_TREE_ALGORITHM
#define HULL_TREE_ALGORITHM

#include <vector>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <cmath>
#include <omp.h>

#include "../geometric_helpers.hh"
#include "../merge_hull.hh"
#include "../sequential/andrew_algorithm.hh"
#include "../algorithm_interfaces/convex_hull_parallel_algorithm.hh"
#include "../data_structures/hull_tree_convex_hull_representation.hh"

#define deb(a)
#define deb2(a)

using namespace std;

class HullTreeAlgorithm : public ConvexHullParallelAlgorithm {

public:
	HullTreeAlgorithm(int threads)
			: ConvexHullParallelAlgorithm(threads) { }

	// Function which calculates a convex hull of a given points set.
	shared_ptr<HullWrapper> convex_hull(vector<POINT*>& points) override {
		omp_set_nested(1);


		// return nullptr;
		shared_ptr<vector<POINT*> > upper_points = convex_points(points, 1);
		deb(printf("----- upper done\n");)
		shared_ptr<vector<POINT*> > lower_points = convex_points(points, 0);
		deb(printf("----- lower done\n");)

		// return nullptr;


		shared_ptr<VectorConvexHullRepresentation> lower_hull = shared_ptr<VectorConvexHullRepresentation>(new VectorConvexHullRepresentation(lower_points, false));
		shared_ptr<VectorConvexHullRepresentation> upper_hull = shared_ptr<VectorConvexHullRepresentation>(new VectorConvexHullRepresentation(upper_points, true));
		shared_ptr<HullWrapper> ret = shared_ptr<HullWrapper>(new HullWrapper(upper_hull, lower_hull));
		return ret;
	}

	shared_ptr<vector<POINT*> > convex_points(vector<POINT*>& points, bool is_upper) {
		int n = points.size();
		d = ceil((float) n / threads);

		shared_ptr<HullTreeConvexHullRepresentation> res = convex_points_rec(points, 0, n - 1, is_upper);
		return res->get_hull();
	}

	shared_ptr<HullTreeConvexHullRepresentation> convex_points_rec(vector<POINT*>& points, int start, int end, bool is_upper) {
		deb2(printf("rec %d %d %d\n", start, end, d);)
		int n = end - start + 1;
		if (n <= d) {
			// Executing sequential version.
			shared_ptr<POINTS> convex_hull_points;
			vector<POINT*> working_points;
			for (int i = start; i <= end; i++) {
				working_points.push_back(points[i]);
			}
			if (is_upper) {
				convex_hull_points = sequential_algorithm->upper_convex_hull(working_points);
			}
			else {
				convex_hull_points = sequential_algorithm->lower_convex_hull(working_points);
			}
			return shared_ptr<HullTreeConvexHullRepresentation>(new HullTreeConvexHullRepresentation(convex_hull_points, is_upper));
		}


		int N = ceil((double) n / d);
		int sqrt_N = ceil(sqrt(N));

		shared_ptr<HullTreeConvexHullRepresentation>*
				partial_results = new shared_ptr<HullTreeConvexHullRepresentation>[sqrt_N];
		shared_ptr<HullTreeConvexHullRepresentation>*
				cut_hulls = new shared_ptr<HullTreeConvexHullRepresentation>[sqrt_N];

		pair<int, int>* intersections = new pair<int, int>[sqrt_N];

		#pragma omp parallel num_threads(sqrt_N)
		// for (int id = 0; id < sqrt_N; id++)
		{
			int id = omp_get_thread_num();
			pair<int, int> range = get_range(n, sqrt_N, id);
			// Shifting range values.
			range.first += start;
			range.second += start;

			deb(printf("start %d %d    %d\n",start, end, id);)
			partial_results[id] = convex_points_rec(points, range.first, range.second, is_upper);
			deb2(if (start == 0 && end == 999999) {
				printf("%d\n", id);
				partial_results[id]->print();
				printf("\n");
			})
			deb(printf("stop %d %d     %d\n", start, end, id);)

			// if (id == 0) {
			// 	partial_results[id]->print();
			// }

		// }

		// for (int id = 0; id < sqrt_N; id++)
		// {

			#pragma omp barrier

			deb(printf("before intersection %d %d     %d\n", start, end, id);)
			intersections[id] = get_intersection(partial_results, sqrt_N, id, is_upper);
			deb2(if (start == 0 && end == 999999) {
				printf("------- %d %d %d\n\n", id, intersections[id].first, intersections[id].second);
			});
			deb(printf("intersection done\n");)
			// cut_hulls[id]->print();
			deb(printf("done printing\n");)

		// }

		// for (int id = 0; id < sqrt_N; id++)
		// {

			#pragma omp barrier
			cut_hulls[id] = cut(partial_results[id], intersections[id]);
			deb(printf("after cut\n");)
		}


		auto res = HullTreeConvexHullRepresentation::merge_hulls(cut_hulls, sqrt_N);

		deb(printf("merge done\n");)

		delete [] partial_results;
		delete [] cut_hulls;


		return res;
	}

	// Calculates parts of the convex hull taken to the result, based on other hulls.
	// shared_ptr<HullTreeConvexHullRepresentation>
	pair<int, int> get_intersection(
			shared_ptr<HullTreeConvexHullRepresentation>* hulls,
			int m,
			int index,
			bool is_upper) {

		int type = is_upper ? 1 : -1;
		int leftmost, rightmost;
		double steepest_left, steepest_right;

		// if (index == 0) return HullTreeConvexHullRepresentation::empty_hull(true);

		tie(leftmost, steepest_left) = get_sidemost(hulls, index, m, is_upper, true);
		deb(printf("left done %d\n", index);)
		tie(rightmost, steepest_right) = get_sidemost(hulls, index, m, is_upper, false);
		deb(printf("right done %d\n", index);)

		deb(printf("%d    %d %d\n", index, leftmost, rightmost);)

		if((type*leftmost < type*rightmost) || ((leftmost == rightmost) && (type*steepest_left <= type*steepest_right))) {
			// return HullTreeConvexHullRepresentation::empty_hull(hulls[index]->is_upper());
			return make_pair(-1, -1);
		}

		return make_pair(leftmost, rightmost);
	}

	shared_ptr<HullTreeConvexHullRepresentation> cut(shared_ptr<HullTreeConvexHullRepresentation>& hull, pair<int, int> intersection) {


		// if((type*leftmost < type*rightmost) || ((leftmost == rightmost) && (type*steepest_left <= type*steepest_right))) {
		if (intersection.first == -1) {
			return HullTreeConvexHullRepresentation::empty_hull(hull->is_upper());
		}

		deb(printf("before trim\n");)

		return hull->trim(intersection.first, intersection.second);
	}

	pair<int, double> get_sidemost(shared_ptr<HullTreeConvexHullRepresentation>* hulls,
			int index,
			int m,
			bool is_upper,
			bool is_left) {

		int start, end;
		if (is_left) {
			start = 0;
			end = index - 1;
		} else {
			start = index + 1;
			end = m - 1;
		}

		deb(int orig_m = m;)

		m = end - start + 1;

		if (start > end) {
			if (is_left) {
				return make_pair(hulls[index]->find_leftmost_point(), 4000000000000);
			} else {
				return make_pair(hulls[index]->find_rightmost_point(), -4000000000000);
			}
		}

		deb(printf("_ %d %d %d\n", index, start, end);)

		// Calculating power of 2 greater or equal than m.
		// TODO(matalek): think whether we can get rid of it.
		int m_rounded = 2;
		while (m_rounded < m) {
			m_rounded <<= 1;
		}

		int* sidemosts = new int[2 * m_rounded];
		double* steepests = new double[2 * m_rounded];

		// printf("parallel\n");
		// Filling out starting actual nodes.
		#pragma omp parallel num_threads(m)
		// for (int id = 0; id < m; id++)
		{
			int id = omp_get_thread_num();
			int left_hull = min(start + id, index);
			int right_hull = max(start + id, index);
			deb(printf("before tangent %d %d %d\n", left_hull, right_hull, orig_m);
			printf("%d\n", !!hulls);
			hulls[right_hull]->print();
			printf("!\n");
			hulls[left_hull]->print();
			printf("!\n");)

			// hulls[right_hull]->print();


			// auto l = VectorConvexHullRepresentation(hulls[left_hull]->get_hull(), true);
			// auto r = VectorConvexHullRepresentation(hulls[right_hull]->get_hull(), true);


			// printf("%d %d\n", hulls[right_hull]->find_leftmost_point(), r.find_leftmost_point());
			// printf("%d %d\n", hulls[right_hull]->find_rightmost_point(), r.find_rightmost_point());
			// hulls[right_hull]->get_point(1)->print();
			// r.get_point(1)->print();
			// printf("%d %d\n", hulls[right_hull]->go_counter_clockwise(0), r.go_counter_clockwise(0));
			// printf("%d %d\n", hulls[right_hull]->go_clockwise(0), r.go_clockwise(0));


			pair<int,int> tangent =
					is_upper
					? findUpperT(*hulls[left_hull], *hulls[right_hull])
					: findLowerT(*hulls[left_hull], *hulls[right_hull]);
			deb(printf("tan %d %d %d %d\n", left_hull, right_hull, tangent.first, tangent.second);)
			double m = angular_coefficient(tangent, *hulls[left_hull], *hulls[right_hull]);
			sidemosts[m_rounded + id] = is_left ? tangent.second : tangent.first;
			steepests[m_rounded + id] = m;
		}

		// printf("bum %d %d\n", index, sidemosts[m_rounded]);

		// Filling out starting dummy nodes.
		const int DUMMY_VALUE = -1;
		if (m_rounded > m) {
			#pragma omp parallel num_threads(m_rounded - m)
			// for (int id = 0; id < m_rounded - m; id++)
			{
				int id = omp_get_thread_num();
				sidemosts[m_rounded + m + id] = DUMMY_VALUE;
				steepests[m_rounded + m + id] = DUMMY_VALUE;
			}
		}


		int level = m_rounded >> 1;
		while (level > 0) {
			#pragma omp parallel num_threads(level)
			// for (int id = 0; id < level; id++)
			{
				// Comparing values from level * 2.
				int id = omp_get_thread_num();
				int pos = level + id; // Position in results tree.
				int left_pos = pos << 1, right_pos = (pos << 1) + 1;

				if (sidemosts[left_pos] == DUMMY_VALUE) {
					sidemosts[pos] = sidemosts[right_pos];
					steepests[pos] = steepests[right_pos];
				} else if (sidemosts[right_pos] == DUMMY_VALUE) {
					sidemosts[pos] = sidemosts[left_pos];
					steepests[pos] = steepests[left_pos];
				} else {
					// Both values are not dummies.
					sidemosts[pos] = is_upper
							? min(sidemosts[left_pos], sidemosts[right_pos])
							: max(sidemosts[left_pos], sidemosts[right_pos]);
					steepests[pos] = is_upper
							? min(steepests[left_pos], steepests[right_pos])
							: max(steepests[left_pos], steepests[right_pos]);
				}
			}
			level >>= 1;
		}

		// printf("ok\n");
		return make_pair(sidemosts[1], steepests[1]);
	}

private:
	int d;

	pair<int, int> get_range(int n, int groups_cnt, int id) {

		int batch_size = ceil((double) n / groups_cnt);

		int start = batch_size * id;
		int end;
		if (id == groups_cnt - 1) {
			end = n - 1;
		} else {
			end = batch_size * (id + 1) - 1;
		}

		// printf("%d %d %d %d %d\n", n, groups_cnt, id, start, end);

		return make_pair(start, end);
	}


	double angular_coefficient(pair<int,int> tangent, ConvexHullRepresentation &hullA, ConvexHullRepresentation &hullB){
		POINT* first = hullA.get_point(tangent.first);
		POINT* second = hullB.get_point(tangent.second);
		return ((double)(first->y - second->y )) / (double)(first->x - second->x);
	}

};

#endif // HULL_TREE_ALGORITHM
