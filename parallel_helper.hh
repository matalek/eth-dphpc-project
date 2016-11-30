#ifndef PARALLEL_HELPER
#define PARALLEL_HELPER

#include <omp.h>
#include <vector>

using namespace std;

class ParallelHelper {

public:

	// Performs calculating prefix sum in parallel.
	// Currently uses OpenMP and takes O(log n) time adn O(n) processors
	// Due to vector allocation limitations, first element of v will be 0
	// and all other elements will be shifted left.
	// Source: https://www.google.pl/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=0ahUKEwi46q26ktDQAhUHtBQKHQx8DJ4QFggdMAA&url=https%3A%2F%2Felectures.informatik.uni-freiburg.de%2Fportal%2Fdownload%2F3%2F9575%2Fthm14%2520-%2520parallel%2520prefix.pdf&usg=AFQjCNFDDsH5Co7GeDopPTeIH0tLaZ0jLQ&sig2=CMO1VqcaU_xxFSzHmYqjHw
	// TODO(matalek): implement version with O(n/log n) processors.
	static void prefix_sum(vector<int>& v) {
		int n = v.size();
		
		// Calculating log n.
		int log = 0;
		int cur = 1;
		while (cur < n) {
			cur <<= 1;
			++log;

		}
		
		// Phase 1.
		for (int d = 0; d < log; d++) {
			int step = 1 << (d + 1);
			int threads = n / step;
			#pragma omp parallel num_threads(threads)
			{
				int id = omp_get_thread_num();
				int i = id * step;
				int half = step >> 1;
				v[i + step - 1] += v[i + half - 1];
			}
		} 

		int sum_all = v[n - 1];
		v[n - 1] = 0;

		// Phase 2.
		for (int d = log - 1; d >= 0; --d) {
			int step = 1 << (d + 1);
			int threads = n / step;
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

		v.push_back(sum_all);
	}
};

#endif // PARALLEL_HELPER
