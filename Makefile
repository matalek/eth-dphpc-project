# On Mac OS X g++ is by default using clang, which doesn't have
# support for OpenMP.
OS := $(shell uname)
ifeq ($(OS), Darwin)
# Mac OS X
CXX = g++-6
else
CXX = g++
endif
CXXFLAGS = -std=c++11 -Wall -fopenmp
LINKFLAGS = -fopenmp

# Passing strategy to be tested - implemented in %.hh file.
ALGORITHM ?= sequential/convex_hull_graham
SEQUENTIAL ?= 1
THREADS ?= 1
CPPFLAGS = -D ALGORITHM=$(ALGORITHM) -D SEQUENTIAL=${SEQUENTIAL} -D THREADS=${THREADS}

EXEC = tester
SOURCES = $(wildcard *.cc)
LIBRARIES = $(shell find . -type f -name '*.hh')
OBJECTS = $(SOURCES:.cc=.o)

all: tester

tester.o: tester.cc $(LIBRARIES)
tester: tester.o
	$(CXX) $(LINKFLAGS) $^ -o $(EXEC)

clean:
	rm -f $(EXEC) $(OBJECTS)