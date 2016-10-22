# Declaration of variables
CC = g++
CC_FLAGS = -std=c++11 -Wall
 
# File names
EXEC = tester
SOURCES = $(wildcard *.cc)
OBJECTS = $(SOURCES:.cc=.o)
 
# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)
 
# To obtain object files
%.o: %.cc
	$(CC) -c $(CC_FLAGS) $< -o $@
 
# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)