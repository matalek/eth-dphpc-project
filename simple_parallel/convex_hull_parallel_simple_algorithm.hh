#ifndef CONVEX_HULL_PARALLEL_SIMPLE_ALGORITHM
#define CONVEX_HULL_PARALLEL_SIMPLE_ALGORITHM

#include <vector>
#include <memory>
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
		int n = points.size();
		shared_ptr<ConvexHullRepresentation> partial_results[threads];
		// Array for the indexing, containing info on how many points each thread has to put in final result
		int position_array[threads];
		// Array for the indexing, containing info on which position in final result each thread should start writing at
		int final_position_array[threads];
		// Array to store leftmost and rightmost points of each separate hull
		pair<int,int> left_n_right[threads];

		#pragma omp parallel num_threads(threads)
		{	
			// Computing the separate convex hulls
			int id = omp_get_thread_num();
			// Calculating part of points for the given thread.
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

			partial_results[id] = shared_ptr<ConvexHullRepresentation>(new R(convex_hull_points, isUpper));

			int leftmost = partial_results[id] -> find_leftmost_point();
			int rightmost = partial_results[id] -> find_rightmost_point();
			
			// Elements used to compute the special case in which leftmost==rightmost.
			// steepest.first = id in partial results
			// steepest.second = position of the point we computed the tangent with
			pair<int,int> steepest_left;
			pair<int,int> steepest_right;
			LL max_y_left = 0;
			LL max_y_right = 0;

			#pragma omp barrier

			//Find rightmost and leftmost for each pair
			for(int i = 0; i < threads; i++){

				pair<int,int> tangent;

				if(isUpper){
					if(i < id){
					//Current element is at right of the alanyzed ch
						tangent = findUpperT(*partial_results[i], *partial_results[id]);
						//hulls rep counterclockwise
						if(tangent.second < leftmost){
							leftmost = tangent.second;
						}
						if(partial_results[i] -> get_point(tangent.first) -> y > max_y_left){
							steepest_left.first = i;
							steepest_left.second = tangent.first;
							max_y_left = partial_results[i] -> get_point(tangent.first) -> y;
						}
					}
					if(id < i){
					//Current element is at left of the alanyzed ch
						tangent = findUpperT(*partial_results[id], *partial_results[i]);
						if(tangent.first > rightmost){
							rightmost = tangent.first;
						}
						if(partial_results[i] -> get_point(tangent.second) -> y > max_y_right){
							steepest_right.first = i;
							steepest_right.second = tangent.second;
							max_y_right = partial_results[i] -> get_point(tangent.second) -> y;
						}
					}
				}
				else{
					if(i < id){
					//Current element is at right
						tangent = findLowerT(*partial_results[i], *partial_results[id]);
						//hulls rep counterclockwise
						if(tangent.second > leftmost){
							leftmost = tangent.second;
						}
						if(partial_results[i] -> get_point(tangent.first) -> y < max_y_left){
							steepest_left.first = i;
							steepest_left.second = tangent.first;
							max_y_left = partial_results[i] -> get_point(tangent.first) -> y;
						}
					}
					if(id < i){
						tangent = findLowerT(*partial_results[id], *partial_results[i]);
						if(tangent.first < rightmost){
							rightmost = tangent.first;
						}
						if(partial_results[i] -> get_point(tangent.second) -> y < max_y_right){
							steepest_right.first = i;
							steepest_right.second = tangent.second;
							max_y_right = partial_results[i] -> get_point(tangent.second) -> y;
						}
					}
				}
			}

			//Find if current ch takes part to final hull
			if(isUpper){
				//Remove the current hull if not in final
				if(leftmost < rightmost){
					position_array[id] = 0;
				}
				else{
					position_array[id] = leftmost - rightmost + 1;
					left_n_right[id].first = leftmost;
					left_n_right[id].second = rightmost;
				}
			}
			else{
				//Remove the current hull if not in final
				if(leftmost > rightmost){
					position_array[id] = 0;
				}
				else{
					position_array[id] = rightmost - leftmost + 1;
					left_n_right[id].first = leftmost;
					left_n_right[id].second = rightmost;
				}
			}
			#pragma omp barrier
			//Check not single point under line		
			if(leftmost == rightmost && id!=0 && id!=threads-1){

				POINT* curr_point = partial_results[id] -> get_point(leftmost);

				POINT* left_sider = partial_results[steepest_left.first] -> get_point(steepest_left.second);
				POINT* right_sider = partial_results[steepest_right.first] -> get_point(steepest_right.second);
				
				//build line and check wether point stands under it
				double y_diff = (right_sider->y - left_sider->y);
				double x_diff = (right_sider->x - left_sider->x);
				double m = y_diff/x_diff;
				double curr_x = (curr_point->x - right_sider->x);
				double line_y = (m)*(curr_x) + right_sider->y;

				if(isUpper){
					if((LL)line_y >= curr_point->y){
						position_array[id] = 0;
					}
				}
				else{
					if((LL)line_y <= curr_point->y){
						position_array[id] = 0;
					}
				}
			}
		}

		//Build the final position array result
		int accumulator = 0;

		for(int i = 0; i < threads; i++){
			final_position_array[i] = accumulator;
			accumulator += position_array[i];
		}

		//Declare points array for final result
		shared_ptr<vector<POINT*> > result_points = shared_ptr<vector<POINT*> >(new vector<POINT*>(accumulator));

		#pragma omp parallel num_threads(threads)
		{
			int id = omp_get_thread_num();
			//write points into final result

			int start_index = final_position_array[id];
			//start writing from leftmost point
			int leftmost = left_n_right[id].first;

			if(isUpper){
				for(int i = 0; i < position_array[id]; i++){
					result_points -> at(i + start_index) = partial_results[id] -> get_point(leftmost - i);
				}
			}
			else{
				for(int i = 0; i < position_array[id]; i++){
					result_points -> at(i + start_index) = partial_results[id] -> get_point(leftmost + i);
				}
			}
		}

		return result_points;
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
