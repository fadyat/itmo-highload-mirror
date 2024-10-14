#include "combine.hpp"
#include "combine.hpp"

#include "../simple_test.h"

TEST(combine, main) {
    auto mult = [](double a, double b) { return a * b; };
    auto plus = [](double a, double b) { return a + b; };

    double arr[] = {1, 2, 3, 4};

    EXPECT_EQ(0, combine(arr, 0, mult, plus));
    EXPECT_EQ(1, combine(arr, 1, mult, plus));
    EXPECT_EQ(2, combine(arr + 1, 1, mult, plus));

    EXPECT_EQ((1. * 2.) + 3., combine(arr, 3, mult, plus));
    EXPECT_EQ((1 * 2) + (3 * 4), combine(arr, 4, mult, plus));

    EXPECT_EQ((1. + 2.) + (3. + 4.), combine(arr, 4, plus, plus));

    auto plus_with_shift = [](double a, double b) { return a + b + 1; };
    EXPECT_EQ(1, combine(arr, 1, mult, plus_with_shift));
    EXPECT_EQ((1. * 2.) + 3. + 1, combine(arr, 3, mult, plus_with_shift));
    EXPECT_EQ(0, combine(arr, 0, mult, plus_with_shift));
    EXPECT_EQ((1. * 2.), combine(arr, 2, mult, plus_with_shift));
    EXPECT_EQ((1. * 2.) + (3. * 4.) + 1, combine(arr, 4, mult, plus_with_shift));
}

TESTING_MAIN()
