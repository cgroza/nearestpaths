nearestpaths: nearestpaths.cpp dijkstra.cpp
	g++ -std=c++2b dijkstra.cpp nearestpaths.cpp -lbdsg -lsdsl -lomp -lhandlegraph -o nearestpaths

all: nearestpaths
