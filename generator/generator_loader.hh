#ifndef GENERATOR_LOADER
#define GENERATOR_LOADER

#include "generator.hh"
#include "square_generator.hh"
#include "circle_generator.hh"
#include "disk_generator.hh"

Generator* load_generator(string shape) {
	Generator* generator;
	if (shape == "circle") {
		generator = new CircleGenerator();
	} else if (shape == "disk") {
		generator = new DiskGenerator();
	} else { // default - square
		generator = new SquareGenerator();
	}
	return generator;
}

#endif //GENERATOR_LOADER