#include "read_write.hpp"
#include "read_write.hpp"

#include <iostream>
#include <cstring>

#include "../simple_test.h"


TEST(test_writing_and_reading, main) {
    char array[sizeof(int) + sizeof(size_t) + sizeof("Hello world!")]{};
    char* write_ptr = array;
    char const* read_ptr = array;

    write_ptr = write(write_ptr, 10);
    EXPECT_EQ(array + sizeof(int), write_ptr);

    {
        int i;
        read_ptr = read(read_ptr, i);
        EXPECT_EQ(10, i);
    }

    write_ptr = write(write_ptr, size_t(20));
    EXPECT_EQ(array + sizeof(int) + sizeof(size_t), write_ptr);

    {
        size_t i;
        read_ptr = read(read_ptr, i);
        EXPECT_EQ(20ull, i);
    }

    write_ptr = write(write_ptr, "Hello world!");
    EXPECT_EQ(array + sizeof(array), write_ptr);
    {
        char* buffer = nullptr;
        read_ptr = read(read_ptr, buffer);
        EXPECT_EQ(0, strcmp(buffer, "Hello world!"));
        EXPECT_EQ(array + sizeof(array), read_ptr );
        delete[] buffer;
    }
}

TESTING_MAIN()
