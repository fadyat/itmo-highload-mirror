cmake_minimum_required (VERSION 3.8)

project(control-3)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") # CMake already contains W3 in its flags
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer")
endif()

add_executable(test_memory_check src/test_memory_check.cpp)
add_executable(test_smoke_task_1 src/test_smoke_task_1.cpp)
add_executable(test_smoke_task_2 src/test_smoke_task_2.cpp)
add_executable(test_smoke_task_3 src/test_smoke_task_3.cpp)
add_executable(test_smoke_task_3_object src/test_smoke_task_3_object.cpp src/test_smoke_task_3_object_extra.cpp)
