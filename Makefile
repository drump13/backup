CXX = g++
CC = gcc
CFLAGS = -c 
CXXFLAGS = -c  -std=c++11

#CSOURCES = \
#	lcm25/lcm.c 

CXXSOURCES = \
	tree.cpp \
	file_io.cpp\
	debug.cpp\
	lcm_cpp/lcm.cpp

OBJECT = \
	tree.o\
	file_io.o\
	debug.o\
	lcm.o

BINARY = debug \

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