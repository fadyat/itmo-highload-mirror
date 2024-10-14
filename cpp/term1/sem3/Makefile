CXX=g++
CXXFLAGS= -Wall -Werror -Wvla -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer -std=c++20 -fsanitize=address

smoke_task_1.o: src/task_1/concat.cpp
	@$(CXX) $(CXXFLAGS) \
		-c ./src/task_1/concat.cpp \
		-o smoke_task_1.o

smoke_task_1_test.o: src/task_1/test.cpp
	@$(CXX) $(CXXFLAGS) \
		-c ./src/task_1/test.cpp \
		-o smoke_task_1_test.o

smoke_task_1: smoke_task_1.o smoke_task_1_test.o
	@$(CXX) $(CXXFLAGS) \
		smoke_task_1.o \
		smoke_task_1_test.o \
		-o smoke_task_1

smoke_task_2.o: src/task_2/int_vector.cpp
	@$(CXX) $(CXXFLAGS) \
		-c ./src/task_2/int_vector.cpp \
		-o smoke_task_2.o

smoke_task_2_test.o: src/task_2/test.cpp
	@$(CXX) $(CXXFLAGS) \
		-c ./src/task_2/test.cpp \
		-o smoke_task_2_test.o

smoke_task_2: smoke_task_2.o smoke_task_2_test.o
	@$(CXX) $(CXXFLAGS) \
		smoke_task_2.o \
		smoke_task_2_test.o \
		-o smoke_task_2

smoke_task_3.o: src/task_3/node_utils.cpp
	@$(CXX) $(CXXFLAGS) \
		-c ./src/task_3/node_utils.cpp \
		-o smoke_task_3.o

smoke_task_3_test.o: src/task_3/test.cpp
	@$(CXX) $(CXXFLAGS) \
		-c ./src/task_3/test.cpp \
		-o smoke_task_3_test.o

smoke_task_3: smoke_task_3.o smoke_task_3_test.o
	@$(CXX) $(CXXFLAGS) \
		smoke_task_3.o \
		smoke_task_3_test.o \
		-o smoke_task_3

smoke_task_4_first.o: src/task_4/first.cpp
	@$(CXX) $(CXXFLAGS) \
		-c ./src/task_4/first.cpp \
		-o smoke_task_4_first.o

smoke_task_4_second.o: src/task_4/second.cpp
	@$(CXX) $(CXXFLAGS) \
		-c ./src/task_4/second.cpp \
		-o smoke_task_4_second.o

smoke_task_4_test.o: src/task_4/test.cpp
	@$(CXX) $(CXXFLAGS) \
		-c ./src/task_4/test.cpp \
		-o smoke_task_4_test.o

smoke_task_4: smoke_task_4_first.o smoke_task_4_second.o smoke_task_4_test.o
	@$(CXX) $(CXXFLAGS) \
		smoke_task_4_first.o \
		smoke_task_4_second.o \
		smoke_task_4_test.o \
		-o smoke_task_4
