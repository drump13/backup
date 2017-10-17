CXX = g++
CC = gcc
CFLAGS = -c 
CXXFLAGS = -c  -std=c++11

CSOURCES = \

CXXSOURCES =lcm.cpp\
	lcm_debug.cpp 

OBJECT = \
	lcm.o\
	lcm_debug.o

BINARY = lcm_debug \

program: 
#	$(CC) $(CFLAGS) $(CSOURCES)
	$(CXX) $(CXXFLAGS) $(CXXSOURCES)
	$(CXX) $(OBJECT) -o $(BINARY)

all: 
	program


.PHONY: clean
clean:
	$(RM) -rf $(OUTPUTDIR)
	$(RM) ./*.gc??
	$(RM) ./*.o