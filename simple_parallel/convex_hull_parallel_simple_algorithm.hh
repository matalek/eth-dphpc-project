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
		shared_ptr<vector<POINT*> > upper_points = convex_points(points,1);
		shared_ptr<vector<POINT*> > lower_points = convex_points(points,0);

		shared_ptr<VectorConvexHullRepresentation> lower_hull = shared_ptr<VectorConvexHullRepresentation>(new VectorConvexHullRepresentation(lower_points,false));
		shared_ptr<VectorConvexHullRepresentation> upper_hull = shared_ptr<VectorConvexHullRepresentation>(new VectorConvexHullRepresentation(upper_points,true));
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

			double steepest_left = 4000000000;
			double steepest_right = -4000000000;
			double steepest_left_down = -4000000000;
			double steepest_right_down = 4000000000;

			#pragma omp barrier

			//Find rightmost and leftmost for each pair
			for(int i = 0; i < threads; i++){

				pair<int,int> tangent;

				if(isUpper){
					if(i < id){

					//Current element is at right of the alanyzed ch
						tangent = findUpperT(*partial_results[i], *partial_results[id]);
						POINT* first = partial_results[i] -> get_point(tangent.first);
						POINT* second = partial_results[id] -> get_point(tangent.second);
						double m = ((double)(first->y - second->y ))/ (double)(first->x - second->x);
						//hulls rep counterclockwise
						if(tangent.second < leftmost){
							leftmost = tangent.second;
							steepest_left = m;
						}
						if(m <= steepest_left){
							steepest_left = m;
						}
					}
					if(id < i){

					//Current element is at left of the alanyzed ch
						tangent = findUpperT(*partial_results[id], *partial_results[i]);
						POINT* first = partial_results[id] -> get_point(tangent.first);
						POINT* second = partial_results[i] -> get_point(tangent.second);
						double m = ((double)(first->y - second->y ))/ (double)(first->x - second->x);
						if(tangent.first > rightmost){
							rightmost = tangent.first;
							steepest_right = m;
						}
						if(m >= steepest_right){
							steepest_right = m;
						}
					}
				}
				//Lower hull
				else{
					if(i < id){
					//Current element is at right
						tangent = findLowerT(*partial_results[i], *partial_results[id]);
						POINT* first = partial_results[i] -> get_point(tangent.first);
						POINT* second = partial_results[id] -> get_point(tangent.second);
						double m = ((long double)(first->y - second->y ))/ (long double)(first->x - second->x);
						//hulls rep counterclockwise
						if(tangent.second > leftmost){
							leftmost = tangent.second;
							steepest_left_down = m;
						}
						if(m >= steepest_left_down){
							steepest_left_down = m;
						}
					}
					if(id < i){
						tangent = findLowerT(*partial_results[id], *partial_results[i]);
						POINT* first = partial_results[id] -> get_point(tangent.first);
						POINT* second = partial_results[i] -> get_point(tangent.second);
						double m = ((long double)(first->y - second->y ))/ (long double)(first->x - second->x);
						if(tangent.first < rightmost){
							rightmost = tangent.first;
							steepest_right_down = m;
						}
						if(m <= steepest_right_down){
							steepest_right_down = m;
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
			if(leftmost == rightmost){
//printf("id: %d, %lld, %lld\n", id, line_y, curr_point->y);
				// Check if steepest lines form angle < 180'
				if(isUpper){
					if(steepest_left <= steepest_right){
						position_array[id] = 0;
//printf("X");
					}
				}
				else{
					if(steepest_left_down >= steepest_right_down){
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
