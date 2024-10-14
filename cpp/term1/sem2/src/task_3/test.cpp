#include "ladder_combination.hpp"
#include "ladder_combination.hpp"

#include <cassert>
#include <iostream>

#include "../simple_test.h"

TEST(testLadderCombinations, main) {
    EXPECT_EQ(0ull, ladderCombinations(0));

    EXPECT_EQ(1ull, ladderCombinations(1));
    EXPECT_EQ(1ull, ladderCombinations(2));
    EXPECT_EQ(2ull, ladderCombinations(3));
    EXPECT_EQ(3ull, ladderCombinations(4));
    EXPECT_EQ(5ull, ladderCombinations(5));
    EXPECT_EQ(8ull, ladderCombinations(6));

    EXPECT_EQ(6765ull, ladderCombinations(20));
}

TESTING_MAIN()
