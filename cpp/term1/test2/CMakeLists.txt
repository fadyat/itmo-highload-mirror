cmake_minimum_required (VERSION 3.8)

project(control-2)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") # CMake already contains W3 in its flags
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer")
endif()

add_executable(task-1-test src/expression.cpp src/custom_new_delete.cpp src/test_task_1.cpp)
add_executable(task-1-5-test src/expression.cpp src/custom_new_delete.cpp src/test_task_1_5.cpp)
add_executable(task-2-test src/expression.cpp src/custom_new_delete.cpp src/test_task_2.cpp)
add_executable(task-3-test src/expression.cpp src/custom_new_delete.cpp src/test_task_3.cpp)
