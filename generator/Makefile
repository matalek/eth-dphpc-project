CXX = g++
CXXFLAGS = -std=c++11 -Wall
 
EXEC = generator
SOURCES = $(wildcard *.cc)
OBJECTS = $(SOURCES:.cc=.o)

all: generator

generator.o: generator.cc
generator: generator.o
	$(CXX) $^ -o $(EXEC)

clean:
	rm -f $(EXEC) $(OBJECTS)