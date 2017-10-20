CXX = g++
CC = gcc
CFLAGS = -c 
CXXFLAGS = -c  -std=c++11

CSOURCES = \
	lcm.c 

CXXSOURCES = \
	debug.cpp

OBJECT = \
	debug.o\
	lcm.o

BINARY = debug \

program: 
	$(CC) $(CFLAGS) $(CSOURCES)
	$(CXX) $(CXXFLAGS) $(CXXSOURCES)
	$(CXX) $(OBJECT) -o $(BINARY)

all: 
	program


.PHONY: clean
clean:
	$(RM) -rf $(OUTPUTDIR)
	$(RM) ./*.gc??
	$(RM) ./*.o       


#LCM: lcm.c
#	gcc -O3 -o fim_all -DLCMFREQ_COUNT_OUTPUT lcm_freq.c
#	gcc -O3 -o fim_closed lcm.c
#	gcc -O3 -o fim_maximal lcm_max.c




