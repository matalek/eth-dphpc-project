#ifndef HULL_TREE_ALGORITHM
#define HULL_TREE_ALGORITHM

#include <vector>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <cmath>
#include <omp.h>

#include "../geometric_helpers.hh"
#include "../parallel_helper.hh"
#include "../merge_hull.hh"
#include "../sequential/andrew_algorithm.hh"
#include "../algorithm_interfaces/convex_hull_parallel_algorithm.hh"
#include "../data_structures/hull_tree_convex_hull_representation.hh"

using namespace std;

class HullTreeAlgorithm : public ConvexHullParallelAlgorithm {

public:
	HullTreeAlgorithm(int threads)
			: ConvexHullParallelAlgorithm(threads) { }

	// Function which calculates a convex hull of a given points set.
	shared_ptr<HullWrapper> convex_hull(vector<POINT*>& points) override {
		omp_set_nested(1);

		ConvexHullAlgorithm::sequential_time = 0;
		start_time = high_resolution_clock::now();
		shared_ptr<vector<POINT*> > upper_points = convex_points(points, 1);
		start_time = high_resolution_clock::now();
		shared_ptr<vector<POINT*> > lower_points = convex_points(points, 0);

		shared_ptr<VectorConvexHullRepresentation> lower_hull = shared_ptr<VectorConvexHullRepresentation>(new VectorConvexHullRepresentation(lower_points, false));
		shared_ptr<VectorConvexHullRepresentation> upper_hull = shared_ptr<VectorConvexHullRepresentation>(new VectorConvexHullRepresentation(upper_points, true));
		shared_ptr<HullWrapper> ret = shared_ptr<HullWrapper>(new HullWrapper(upper_hull, lower_hull));
		return ret;
	}

private:
	high_resolution_clock::time_point start_time;

	shared_ptr<vector<POINT*> > convex_points(vector<POINT*>& points, bool is_upper) {
		int n = points.size();
		d = ceil((float) n / threads);

		shared_ptr<HullTreeConvexHullRepresentation> res = convex_points_rec(points, 0, n - 1, is_upper);
		return res->get_hull();
	}

	shared_ptr<HullTreeConvexHullRepresentation> convex_points_rec(vector<POINT*>& points, int start, int end, bool is_upper) {
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

			if (start == 0) {
				ConvexHullAlgorithm::sequential_time += duration_cast<microseconds>( high_resolution_clock::now() - start_time ).count();
			}

			return shared_ptr<HullTreeConvexHullRepresentation>(new HullTreeConvexHullRepresentation(convex_hull_points, is_upper));
		}


		int N = ceil((double) n / d);
		int sqrt_N = sqrt(N);
		// TODO(matalek): make it more general and pretty.
		if (N == 32) {
			sqrt_N = 4;
		}
		if (sqrt_N == 1) {
			sqrt_N++;
		}

		shared_ptr<HullTreeConvexHullRepresentation>*
				partial_results = new shared_ptr<HullTreeConvexHullRepresentation>[sqrt_N];
		shared_ptr<HullTreeConvexHullRepresentation>*
				cut_hulls = new shared_ptr<HullTreeConvexHullRepresentation>[sqrt_N];

		pair<int, int>* intersections = new pair<int, int>[sqrt_N];

		#pragma omp parallel num_threads(sqrt_N)
		{

			int id = omp_get_thread_num();
			pair<int, int> range = ParallelHelper::get_range(n, sqrt_N, id);
			// Shifting range values.
			range.first += start;
			range.second += start;

			partial_results[id] = convex_points_rec(points, range.first, range.second, is_upper);

			#pragma omp barrier

			// Calculating intersection with the convex hull of the whole set.
			intersections[id] = get_intersection(partial_results, sqrt_N, id, is_upper);

			#pragma omp barrier

			// Trimming hulls based on calculated earlier intersection.
			cut_hulls[id] = cut(partial_results[id], intersections[id]);
		}

		auto res = HullTreeConvexHullRepresentation::merge_hulls(cut_hulls, sqrt_N);

		delete [] partial_results;
		delete [] cut_hulls;

		return res;
	}

	// Calculates parts of the convex hull taken to the result, based on other hulls.
	pair<int, int> get_intersection(
			shared_ptr<HullTreeConvexHullRepresentation>* hulls,
			int m,
			int index,
			bool is_upper) {

		int type = is_upper ? 1 : -1;
		int leftmost, rightmost;
		double steepest_left, steepest_right;

		tie(leftmost, steepest_left) = get_sidemost(hulls, index, m, is_upper, true);
		tie(rightmost, steepest_right) = get_sidemost(hulls, index, m, is_upper, false);

		if((type*leftmost < type*rightmost) || ((leftmost == rightmost) && (type*steepest_left <= type*steepest_right))) {
			// No point is being taken to the convex hull of the entire set.
			return make_pair(-1, -1);
		}

		return make_pair(leftmost, rightmost);
	}

	shared_ptr<HullTreeConvexHullRepresentation> cut(shared_ptr<HullTreeConvexHullRepresentation>& hull, pair<int, int> intersection) {


		if (intersection.first == -1) {
			return HullTreeConvexHullRepresentation::empty_hull(hull->is_upper());
		}

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

		m = end - start + 1;
		int type = is_upper ? 1 : -1;

		if (start > end) {
			if (is_left) {
				return make_pair(hulls[index]->find_leftmost_point(), type * DBL_MAX);
			} else {
				return make_pair(hulls[index]->find_rightmost_point(), (-type) * DBL_MAX);
			}
		}

		// Calculating power of 2 greater or equal than m.
		// TODO(matalek): think whether we can get rid of it.
		int m_rounded = 2;
		while (m_rounded < m) {
			m_rounded <<= 1;
		}

		int* sidemosts = new int[2 * m_rounded];
		double* steepests = new double[2 * m_rounded];

		// Filling out starting actual nodes.
		#pragma omp parallel num_threads(m)
		{
			int id = omp_get_thread_num();
			int left_hull = min(start + id, index);
			int right_hull = max(start + id, index);

           auto l = VectorConvexHullRepresentation(hulls[left_hull]->get_hull(), true);
           auto r = VectorConvexHullRepresentation(hulls[right_hull]->get_hull(), true);

			pair<int,int> tangent =
					is_upper
					? findUpperT(*hulls[left_hull], *hulls[right_hull])
					// ? findUpperT(l, r)
					: findLowerT(*hulls[left_hull], *hulls[right_hull]);

			double m = angular_coefficient(tangent, *hulls[left_hull], *hulls[right_hull]);
			sidemosts[m_rounded + id] = is_left ? tangent.second : tangent.first;
			steepests[m_rounded + id] = m;
		}

		// Filling out starting dummy nodes.
		const int DUMMY_VALUE = -1;
		if (m_rounded > m) {
			#pragma omp parallel num_threads(m_rounded - m)
			{
				int id = omp_get_thread_num();
				sidemosts[m_rounded + m + id] = DUMMY_VALUE;
				steepests[m_rounded + m + id] = DUMMY_VALUE;
			}
		}

		int level = m_rounded >> 1;
		while (level > 0) {
			#pragma omp parallel num_threads(level)
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
					sidemosts[pos] = ((is_upper && is_left) || (!is_upper && !is_left))
							? min(sidemosts[left_pos], sidemosts[right_pos])
							: max(sidemosts[left_pos], sidemosts[right_pos]);
					steepests[pos] = ((is_upper && is_left) || (!is_upper && !is_left))
							? min(steepests[left_pos], steepests[right_pos])
							: max(steepests[left_pos], steepests[right_pos]);
				}
			}
			level >>= 1;
		}

		return make_pair(sidemosts[1], steepests[1]);
	}

	// Threshold size for the points set size, like in the algorithm.
	int d;

	double angular_coefficient(pair<int,int> tangent, ConvexHullRepresentation &hullA, ConvexHullRepresentation &hullB){
		POINT* first = hullA.get_point(tangent.first);
		POINT* second = hullB.get_point(tangent.second);
		return ((double)(first->y - second->y )) / (double)(first->x - second->x);
	}
};

#endif // HULL_TREE_ALGORITHM
