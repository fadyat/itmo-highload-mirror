cmake_minimum_required (VERSION 3.8)

project(clw-10)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") # CMake already contains W3 in its flags
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer -Wno-maybe-uninitialized")
endif()

add_executable(task-1-test src/test/task_1_test.cpp)
add_executable(task-2-test src/test/task_2_test.cpp)
add_executable(task-2-5-test src/test/task_2_5_test.cpp)
add_executable(task-3-test src/test/task_3_test.cpp)
