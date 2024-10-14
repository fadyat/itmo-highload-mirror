cmake_minimum_required (VERSION 3.8)

project(control-1)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") # CMake already contains W3 in its flags
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wextra-semi -Wvla-extension -O1 -g -fsanitize=address -fno-omit-frame-pointer")
endif()

add_executable(smoke_task_1 src/task_1.cpp src/network.cpp)
add_executable(smoke_task_2 src/task_2.cpp src/network.cpp)
add_executable(smoke_task_3 src/task_3.cpp src/network.cpp)
add_executable(smoke_task_4 src/task_4.cpp src/network.cpp)
add_executable(smoke_task_5 src/task_5.cpp src/network.cpp)
add_executable(smoke_task_6 src/task_6.cpp src/network.cpp)
