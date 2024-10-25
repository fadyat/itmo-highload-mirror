cmake_minimum_required (VERSION 3.8)

project(clw-05)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") # CMake already contains W3 in its flags
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer")
endif()

# Uncomment to work on task 1, string_view
add_executable(string_view_test
    src/task_1/string_view.cpp
    src/task_1/string_view.hpp
    src/task_1/test.cpp
    src/simple_test.h
)

# Uncomment to work on task 2, shared_buffer
add_executable(shared_buffer_test src/task_2/shared_buffer.cpp src/task_2/test_shared_buffer.cpp)

# Uncomment to work on task 2, lazy_string
add_executable(lazy_string_test src/task_2/shared_buffer.cpp src/task_2/lazy_string.cpp src/task_2/test_lazy_string.cpp)

# Uncomment to work on task 2, lazy_string_access_op
add_executable(lazy_string_access_op_test src/task_2/shared_buffer.cpp src/task_2/lazy_string.cpp src/task_2/test_access_operator.cpp)
