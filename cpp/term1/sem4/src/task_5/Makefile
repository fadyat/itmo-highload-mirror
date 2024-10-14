CXX=g++
CXXFLAGS= -Wall -Werror -Wvla -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer -std=c++20 -fsanitize=address

CC=gcc

test.o: test.cpp
	$(CXX) $(CXXFLAGS) -c test.cpp

sum.o: sum.c
	$(CC) -c sum.c

smoke_task_5: test.o sum.o
	$(CXX) $(CXXFLAGS) -o smoke_task_5 test.o sum.o
