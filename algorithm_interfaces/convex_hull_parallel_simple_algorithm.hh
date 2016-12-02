#ifndef CONVEX_HULL_PARALLEL_SIMPLE_ALGORITHM
#define CONVEX_HULL_PARALLEL_SIMPLE_ALGORITHM

#include <vector>
#include <memory>
#include "../geometric_helpers.hh"
#include "convex_hull_sequential_algorithm.hh"
#include "convex_hull_parallel_algorithm.hh"

using namespace std;

template <class R>
class ConvexHullParallelSimpleAlgorithm : public ConvexHullParallelAlgorithm {

public:
	ConvexHullParallelSimpleAlgorithm(int threads)
		: ConvexHullParallelAlgorithm(threads) { }

	// Function which calculates a convex hull of a given points set.
	shared_ptr<vector<POINT*> > convex_hull(vector<POINT*>& points) override {
		
		
		shared_ptr<vector<POINT*> > upper_points = convex_points(points,1);
		shared_ptr<vector<POINT*> > lower_points = convex_points(points,0);
		for(unsigned int i = (lower_points.get() -> size()) - 2; i > 0 ; i--){
			upper_points.get() -> push_back(lower_points -> at(i));
		}
		
		return upper_points;
	}


	shared_ptr<vector<POINT*> > convex_points(vector<POINT*>& points, bool isUpper){
		int n = points.size();
		shared_ptr<ConvexHullRepresentation> partial_results[threads];
		int position_array[threads];
		int final_position_array[threads];
		//Array to store leftmost and rightmost
		pair<int,int> left_n_right[threads];

		#pragma omp parallel num_threads(threads)
		{	
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
			partial_results[id] = shared_ptr<ConvexHullRepresentation>(new R(convex_hull_points));

			int leftmost = partial_results[id] -> find_leftmost_point();
			int rightmost = partial_results[id] -> find_rightmost_point();

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
					}
					if(id < i){
					//Current element is at left of the alanyzed ch
						tangent = findUpperT(*partial_results[id], *partial_results[i]);
						if(tangent.first > rightmost){
							rightmost = tangent.first;
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
					}
					if(id < i){
						tangent = findLowerT(*partial_results[id], *partial_results[i]);
						if(tangent.first < rightmost){
							rightmost = tangent.first;
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
		}

		//Build the final position array result
		int accumulator = 0;
		final_position_array[0] = 0;

		for(int i = 0; i < threads; i++){
			accumulator += position_array[i];
			final_position_array[i+1] = accumulator;
		}

		//Declare points array for final result
		shared_ptr<vector<POINT*> > result_points = shared_ptr<vector<POINT*> >(new vector<POINT*>(accumulator));
		//vector<POINT*> final_result(accumulator);

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
