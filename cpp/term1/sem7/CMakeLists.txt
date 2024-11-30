cmake_minimum_required (VERSION 3.8)

project(clw-07)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") # CMake already contains W3 in its flags
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra-semi -O1 -g -fno-omit-frame-pointer")
endif()

add_executable(task-1-test src/task_1/json_value.cpp src/task_1/test_json_value.cpp)
add_executable(task-2-test src/task_2/leak_detector.cpp src/task_2/test_leak_detector.cpp)
add_executable(task-2-alloc-dealloc-test src/task_2/leak_detector.cpp src/task_2/test_alloc_dealloc.cpp)
add_executable(task-2-extensive-allocation-test src/task_2/leak_detector.cpp src/task_2/test_extensive_allocation.cpp)
