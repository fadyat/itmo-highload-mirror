cmake_minimum_required (VERSION 3.8)

project(clw-06)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") # CMake already contains W3 in its flags
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer")
endif()

include_directories(src)

add_executable(task-1-1-test src/task_1/task_1_1_test.cpp src/task_1/my_ostream.cpp src/task_1/my_ostream_con.cpp)
add_executable(task-1-2-test src/task_1/task_1_2_test.cpp src/task_1/my_ostream.cpp src/task_1/my_ostream_file.cpp)
add_executable(task-1-3-test src/task_1/task_1_3_test.cpp src/task_1/my_ostream.cpp src/task_1/my_ostream_con.cpp src/task_1/my_ostream_file.cpp src/task_1/my_ostream_combo.cpp)
add_executable(task-2-test src/task_2/test.cpp src/task_2/corpus_index.cpp)
add_executable(task-3-test src/task_3/test.cpp src/task_3/get_size.cpp)
add_executable(task-3-test-alignment src/task_3/test_alignment.cpp  src/task_3/get_size.cpp)
