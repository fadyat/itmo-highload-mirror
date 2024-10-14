cmake_minimum_required(VERSION 3.8)
project(cls03)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") # CMake already contains W3 in its flags
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wvla -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer")
endif()

# Uncomment to work on task 1
#add_executable(task-1-concat src/task_1/concat.cpp src/task_1/test.cpp)

# Uncomment to work on task 2
#add_executable(task-2-int-vector src/task_2/int_vector.cpp src/task_2/test.cpp)

# Uncomment to work on task 3
#add_executable(task-3-node-utils src/task_3/node_utils.cpp src/task_3/test.cpp)

# Uncomment to work on task 4
#add_executable(task-4-singleton src/task_4/first.cpp src/task_4/second.cpp src/task_4/singleton.cpp src/task_4/test.cpp)
