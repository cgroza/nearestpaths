nearestpaths: nearestpaths.cpp
	g++ -std=c++2b  nearestpaths.cpp -lbdsg -lsdsl -lomp -lhandlegraph -o nearestpaths

all: nearestpaths
