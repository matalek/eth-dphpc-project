#include <vector>
#include "geometric_helpers.hh"
#include "convex_hull_graham.hh"
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <string>
#include <iterator>

int main(int argc, char *argv[]) {

	using namespace std;

	// Reading from standard input points.
	vector<POINT> points(argc-1);
	for (int i = 1 ; i < argc ; i++) {
		string input = argv[i];
		istringstream ss(input);
		string token;
		int parser [2];
		int count = 0;
		while(std::getline(ss, token, ',')) {
			parser[count++] = stoi(token);
		}
		points[i-1] = POINT(parser[0], parser[1]);
	}
	
	printf("Number of resulting points: ");
	auto convex_hull_points = convex_hull(points);
	printf("%lu\n", convex_hull_points.size());
	for (POINT* point : convex_hull_points) {
		point->print();
	}

	return 0;
}
