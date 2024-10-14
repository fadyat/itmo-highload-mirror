CXX=g++
CXXFLAGS=-Wall -Werror -std=c++20

all: smoke

matrices: main.cpp matrices.cpp matrices.hpp
	$(CXX) $(CXXFLAGS) main.cpp matrices.cpp -o matrices

smoke: matrices
	cd smoke_test && ./smoke_test.sh ../matrices
