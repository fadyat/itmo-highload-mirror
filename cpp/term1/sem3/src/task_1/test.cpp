#include "concat.hpp"
#include "concat.hpp"

#include <cstring>

#include "../simple_test.h"


TEST(concat, test_ints) {
    int first[] = {0, 1, 2, 3};
    int second[] = {4, 5, 6, 7, 8, 9};

    size_t result_size;
    char const* result = concat(
            reinterpret_cast<char const*>(first),
            sizeof(first),
            reinterpret_cast<char const*>(second),
            sizeof(second),
            result_size
    );

    EXPECT_EQ(10ul * sizeof(int), result_size);
    int const* int_result = reinterpret_cast<int const*>(result);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(i, int_result[i]) << "int_result[" << i << "] != " << i;
    }

    delete[] result;
}

TEST(concat, test_chars) {
    size_t result_size;
    char const* result = concat(
            "Hello, World!",
            7,
            "Everyone!",
            10, // we need to get that \0 byte
            result_size
    );

    EXPECT_EQ(17ul, result_size);
    EXPECT_EQ(0, strcmp(result, "Hello, Everyone!"));

    delete[] result;
}

TESTING_MAIN()
