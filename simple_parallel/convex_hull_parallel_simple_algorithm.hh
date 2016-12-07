#ifndef CONVEX_HULL_PARALLEL_SIMPLE_ALGORITHM
#define CONVEX_HULL_PARALLEL_SIMPLE_ALGORITHM

#include <vector>
#include <memory>
#include <cfloat>
#include "../geometric_helpers.hh"
#include "../algorithm_interfaces/convex_hull_sequential_algorithm.hh"
#include "../algorithm_interfaces/convex_hull_parallel_algorithm.hh"

using namespace std;

template <class R>
class ConvexHullParallelSimpleAlgorithm : public ConvexHullParallelAlgorithm {

public:
	ConvexHullParallelSimpleAlgorithm(int threads)
		: ConvexHullParallelAlgorithm(threads) { }

	// Function which calculates a convex hull of a given points set.
	shared_ptr<HullWrapper> convex_hull(vector<POINT*>& points) override {

		shared_ptr<VectorConvexHullRepresentation> lower_hull = shared_ptr<VectorConvexHullRepresentation>(new VectorConvexHullRepresentation(convex_points(points,0),false));
		shared_ptr<VectorConvexHullRepresentation> upper_hull = shared_ptr<VectorConvexHullRepresentation>(new VectorConvexHullRepresentation(convex_points(points,1),true));
		shared_ptr<HullWrapper> ret = shared_ptr<HullWrapper>(new HullWrapper(upper_hull, lower_hull));
		return ret;
	}


	shared_ptr<vector<POINT*> > convex_points(vector<POINT*>& points, bool isUpper){
		int type = isUpper ? 1 : -1;
		shared_ptr<ConvexHullRepresentation> partial_results[threads];
		// Array for the indexing, containing info on how many points each thread has to put in final result
		int position_array[threads];
		// Array for the indexing, containing info on which position in final result each thread should start writing at
		int final_position_array[threads];
		// Array to store leftmost and rightmost points of each separate hull
		pair<int,int> left_n_right[threads];

		// PARALLEL SECTION
		#pragma omp parallel num_threads(threads)
		{	
			
			int id = omp_get_thread_num();

			// Computing the separate convex hulls
			partial_results[id] = build_sequential_hull(id, points, isUpper);
			// Partial convex hulls built

			int leftmost = partial_results[id] -> find_leftmost_point();
			int rightmost = partial_results[id] -> find_rightmost_point();

			double steepest_left = type * DBL_MAX;
			double steepest_right = (-type) * DBL_MAX;

			#pragma omp barrier
			
			//Find rightmost and leftmost for each pair
			for(int i = 0; i < threads; i++){
				if(i==id){
					continue;
				}
				int left_hull = min(i,id);
				int right_hull = max(i,id);
				pair<int,int> tangent = isUpper ? findUpperT(*partial_results[left_hull], *partial_results[right_hull]) : findLowerT(*partial_results[left_hull], *partial_results[right_hull]);
				double m = angular_coefficient(tangent, *partial_results[left_hull], *partial_results[right_hull]);
				if(i < id){
				//Current element is at right of the alanyzed ch
					if(type*tangent.second < type*leftmost){
						leftmost = tangent.second;
						//steepest_left = m;
					}
					if(type*m < type*steepest_left){
						steepest_left = m;
					}
				}
				else{
				//Current element is at left of the alanyzed ch
					if(type*tangent.first > type*rightmost){
						rightmost = tangent.first;
						//steepest_right = m;
					}
					if(type*m > type*steepest_right){
						steepest_right = m;
					}
				}
			}

			//Find if current ch takes part to final hull

			//Remove the current hull if not in final
			if((type*leftmost < type*rightmost) || ((leftmost == rightmost) && (type*steepest_left <= type*steepest_right))){
				position_array[id] = 0;
			}
			else{
				position_array[id] = abs(leftmost - rightmost) + 1;
				left_n_right[id].first = leftmost;
				left_n_right[id].second = rightmost;
			}
		}
		// END OF PARALLEL SECTION

		// SEQUENTIAL SECTION
		// Build the final position array result and indexing
		int accumulator = 0;
		for(int i = 0; i < threads; i++){
			final_position_array[i] = accumulator;
			accumulator += position_array[i];
		}

		//Declare points array for final result
		shared_ptr<vector<POINT*> > result_points = shared_ptr<vector<POINT*> >(new vector<POINT*>(accumulator));
		//END OF SEQUENTIAL SECTION

		// PARALLEL SECITON
		#pragma omp parallel num_threads(threads)
		{
			// write points into final vector
			int id = omp_get_thread_num();
			int start_index = final_position_array[id];
			int leftmost = left_n_right[id].first;
			int start_position = isUpper ? (accumulator - 1) : 0;

			for(int i = 0; i < position_array[id]; i++){
				result_points -> at(start_position + (-type)*(i + start_index)) = partial_results[id] -> get_point(leftmost + (-type)*i);
			}
		}
		// END OF PARALLEL SECITON

		return result_points;
}

private:
	shared_ptr<ConvexHullRepresentation> build_sequential_hull(int id, vector<POINT*>& points, bool isUpper){
		int n = points.size();
		pair<int, int> range = get_range(n, id);
		vector<POINT*> working_points;
		for (int i = range.first; i < range.second; i++) {
			working_points.push_back(points[i]);
		}
		// Calculating convex hull of the appropriate part of points.
		shared_ptr<vector<POINT*> > convex_hull_points;
		
		if(isUpper){
			convex_hull_points = sequential_algorithm->upper_convex_hull(working_points);
		}
		else{
			convex_hull_points = sequential_algorithm->lower_convex_hull(working_points);
		}

		return shared_ptr<ConvexHullRepresentation>(new R(convex_hull_points, isUpper));
}

private:
	double angular_coefficient(pair<int,int> tangent, ConvexHullRepresentation &hullA, ConvexHullRepresentation &hullB){
		POINT* first = hullA.get_point(tangent.first);
		POINT* second = hullB.get_point(tangent.second);
		return ((double)(first->y - second->y )) / (double)(first->x - second->x);
}

private:
	pair<int, int> get_range(int n, int id) {
		int batch_size = n / threads;

		int start = batch_size * id;
		int end;
		if (id == threads - 1) {
			end = n;
		} else {
			end = batch_size * (id + 1);
		}

		return make_pair(start, end);
	}
};

#endif // CONVEX_HULL_PARALLEL_SIMPLE_ALGORITHM
