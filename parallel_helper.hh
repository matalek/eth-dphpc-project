#ifndef PARALLEL_HELPER
#define PARALLEL_HELPER

#include <omp.h>
#include <vector>
#include <cmath>

using namespace std;

class ParallelHelper {

public:

	// Performs calculating prefix sum in parallel.
	// Currently uses OpenMP and takes O(log n) time adn O(n) processors.
	// Source: https://www.google.pl/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=0ahUKEwi46q26ktDQAhUHtBQKHQx8DJ4QFggdMAA&url=https%3A%2F%2Felectures.informatik.uni-freiburg.de%2Fportal%2Fdownload%2F3%2F9575%2Fthm14%2520-%2520parallel%2520prefix.pdf&usg=AFQjCNFDDsH5Co7GeDopPTeIH0tLaZ0jLQ&sig2=CMO1VqcaU_xxFSzHmYqjHw
	// TODO(matalek): implement version with O(n/log n) processors.
	static int* prefix_sum(int* numbers, int n) {
		// Calculating log n.
		int log = 0;
		int cur = 1;
		while (cur < n) {
			cur <<= 1;
			++log;
		}

		int* v = new int[cur];
		int* res = new int[n];

		#pragma omp parallel num_threads(n)
		{
			int id = omp_get_thread_num();
			v[id] = numbers[id];
		}

		if (n < cur) {
			#pragma omp parallel num_threads(cur - n)
			{
				int id = omp_get_thread_num();
				v[n + id] = 0;
			}
		}

		// Phase 1.
		for (int d = 0; d < log; d++) {
			int step = 1 << (d + 1);
			int threads = cur / step;
			#pragma omp parallel num_threads(threads)
			{
				int id = omp_get_thread_num();
				int i = id * step;
				int half = step >> 1;
				v[i + step - 1] += v[i + half - 1];
			}
		}

		int sum_all = v[cur - 1];
		v[cur - 1] = 0;

		// Phase 2.
		for (int d = log - 1; d >= 0; --d) {
			int step = 1 << (d + 1);
			int threads = cur / step;
			#pragma omp parallel num_threads(threads)
			{
				int id = omp_get_thread_num();
				int i = id * step;
				int half = step >> 1;
				int temp = v[i + half - 1];
				v[i + half - 1] = v[i + step - 1];
				v[i + step - 1] += temp;
			}
		}

		#pragma omp parallel num_threads(n - 1)
		{
			int id = omp_get_thread_num();
			res[id] = v[id + 1];
		}
		res[n - 1] = sum_all;

		delete[] v;
		return res;
	}

	// Calculates appropriate range of points for the given thread
	// (divides points equally between threads).
	static pair<int, int> get_range(int n, int groups_cnt, int id) {
		int batch_size = ceil((double) n / groups_cnt);

		int start = batch_size * id;
		int end;
		if (id == groups_cnt - 1) {
			end = n - 1;
		} else {
			end = batch_size * (id + 1) - 1;
		}

		return make_pair(start, end);
	}
};

#endif // PARALLEL_HELPER
