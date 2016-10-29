CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Passing strategy to be tested - implemented in %.hh file.
STRATEGY ?= "convex_hull_graham"
CPPFLAGS = -D ALGORITHM=$(ALGORITHM)
 
EXEC = tester
SOURCES = $(wildcard *.cc)
LIBRARIES = $(shell find . -type f -name '*.hh')
OBJECTS = $(SOURCES:.cc=.o)

all: tester

tester.o: tester.cc $(LIBRARIES)
tester: tester.o
	$(CXX) $^ -o $(EXEC)

clean:
	rm -f $(EXEC) $(OBJECTS)