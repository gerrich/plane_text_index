CXXFLAGS=-O0 -g

all:
	g++ -o bi-stat-score bi-stat-score.cpp 
	g++ -o bi-stat-build-index bi-stat-build-index.cpp
